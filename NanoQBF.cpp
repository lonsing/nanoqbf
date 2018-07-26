//
// Created by vedad on 16/06/18.
//

#include "NanoQBF.h"
#include "types/Formula.h"
#include "auxutils.h"

NanoQBF::NanoQBF(Formula* formula, const Options* options) :
formula_(formula),
options_(options),
iteration_(0),
forced_prune_a_(false),
forced_prune_b_(false)
{
  if(formula_->numQuants() == 0) return;
  
  const Quant* qfirst = formula_->getQuant(0);
  if(qfirst->type == QuantType::EXISTS)
    solver_a_.reserveVars(qfirst->size);
  else
    solver_b_.reserveVars(qfirst->size);
}

NanoQBF::~NanoQBF()
{
  for(const auto & a : subformula_solutions_a_)
    Assignment::destroy_assignment(a);
  for(const auto & a : subformula_solutions_b_)
    Assignment::destroy_assignment(a);
  
  for(const auto & av : vars_a_)
    Assignment::destroy_assignment(av.first);
  for(const auto & av : vars_b_)
    Assignment::destroy_assignment(av.first);
  delete options_;
}

int NanoQBF::solve()
{
  int res = initA();
  if(res != 0) return res;
  
  LOG("Entering main loop");
  
  double time_solve_a = 0;
  double time_solve_b = 0;
  double time_complete_a = 0;
  double time_complete_b = 0;
  
  double time_tmp = 0;
  double time_0 = read_cpu_time();
  while(true)
  {
    iteration_++;
    if(iteration_ % 20 == 0)
      printf("c Iteration %d\n", iteration_);
    
    if(read_cpu_time() - time_0 > options_->time_limit)
    {
      printf("c NanoQBF ran out of time, retrning unknown\n");
      return 0;
    }
    
    LOG("c subformuals A:   %lu", subformula_vars_a_.size());
    LOG("c subformuals B:   %lu", subformula_vars_b_.size());
    LOG("c solve time A:    %f", time_solve_a);
    LOG("c solve time B:    %f", time_solve_b);
    LOG("c complete time A: %f", time_complete_a);
    LOG("c complete time B: %f", time_complete_b);
  
    time_tmp = read_cpu_time();
    int res_a = solver_a_.solve();
    while(res_a == -1)
    {
      printf("c Solver A went out of memory, pruning and trying again\n");
      pruneA(), completeA(), forced_prune_b_ = true;
      res_a = solver_a_.solve();
    }
    if(res_a == 20) return 20;
    time_solve_a += read_cpu_time() - time_tmp;
  
    pruneCheckB();
    
    time_tmp = read_cpu_time();
    completeB();
    time_complete_b += read_cpu_time() - time_tmp;
    
    time_tmp = read_cpu_time();
    int res_b = solver_b_.solve();
    while(res_b == -1)
    {
      printf("c Solver B went out of memory, pruning and trying again\n");
      pruneB(), completeB(), forced_prune_a_ = true;
      res_b = solver_b_.solve();
    }
    if(res_b == 20) return 10;
    time_solve_b += read_cpu_time() - time_tmp;
    
    pruneCheckA();
    
    time_tmp = read_cpu_time();
    completeA();
    time_complete_a += read_cpu_time() - time_tmp;
  }
}

int NanoQBF::initA()
{
  SatSolver warmup_solver;
  for(unsigned ci = 0; ci < formula_->numClauses(); ci++)
    warmup_solver.addClause(formula_->getClause(ci));
  
  std::vector<Lit> values;
  
  for(unsigned wi = 0; wi < options_->warmup_samples; wi++)
  {
    // the problem is unsatisfiable or we are warmed up
    int res = warmup_solver.solve();
    if(res == 20)
      return wi ? 0 : 20;
    if(res == 10 && formula_->numQuants() == 1 && formula_->getQuant(0)->type == QuantType::EXISTS)
      return 10;
    
    values.clear();
    
    for(unsigned qi = 0; qi < formula_->numQuants(); qi++)
    {
      const Quant* quant = formula_->getQuant(qi);
      if(quant->type == QuantType::EXISTS) continue;
      for(const_var_iterator v_iter = quant->begin(); v_iter != quant->end(); v_iter++)
        values.push_back(warmup_solver.getValue(*v_iter));
    }
    Assignment* assignment = Assignment::make_assignment(values);
    std::cout << *assignment << std::endl;
    
    extendA(assignment);
    
    for(unsigned vi = 0; vi < values.size(); vi++)
      values[vi] = -values[vi];
    
    warmup_solver.addClause(values);
  }
  
  if(!options_->structured_warmup) return 0;
  
  for(unsigned qi = 0; qi < formula_->numQuants(); qi++)
  {
    const Quant* quant = formula_->getQuant(qi);
    if(quant->type == QuantType::EXISTS) continue;
    Assignment* assignment_n = Assignment::make_assignment(formula_->numUniversal());
    Assignment* assignment_p = Assignment::make_assignment(formula_->numUniversal());
    
    unsigned pos = 0;
    for(unsigned qni = 0; qni < formula_->numQuants(); qni++)
    {
      const Quant* quant_i = formula_->getQuant(qni);
      if(quant_i->type == QuantType::EXISTS) continue;
      bool valn = (qi == qni);
      for(unsigned vi = 0; vi < quant_i->size; vi++)
      {
        assignment_n->set(pos + vi, valn);
        assignment_p->set(pos + vi, !valn);
      }
      pos += quant_i->size;
    }

    assignment_n->rehash();
    assignment_p->rehash();
    
    extendA(assignment_n);
    extendA(assignment_p);
  }
  
  for(unsigned qi = 0; qi < formula_->numQuants(); qi++)
  {
    const Quant* quant = formula_->getQuant(qi);
    if(quant->type == QuantType::FORALL) continue;
    Assignment* assignment_n = Assignment::make_assignment(formula_->numExistential());
    Assignment* assignment_p = Assignment::make_assignment(formula_->numExistential());
    
    unsigned pos = 0;
    for(unsigned qni = 0; qni < formula_->numQuants(); qni++)
    {
      const Quant* quant_i = formula_->getQuant(qni);
      if(quant_i->type == QuantType::FORALL) continue;
      bool valn = (qi == qni);
      for(unsigned vi = 0; vi < quant_i->size; vi++)
      {
        assignment_n->set(pos + vi, valn);
        assignment_p->set(pos + vi, !valn);
      }
      pos += quant_i->size;
    }
  
    assignment_n->rehash();
    assignment_p->rehash();
    
    extendB(assignment_n);
    extendB(assignment_p);
  }
  
  return 0;
}


void NanoQBF::completeA()
{
  Assignment* assignment = Assignment::make_assignment(formula_->numUniversal());
  
  unsigned counter = 0;
  
  for(unsigned si = 0; si < subformula_vars_b_.size(); si++)
  {
    const std::vector<Var>& vars = subformula_vars_b_[si];
    unsigned qi = (unsigned)(formula_->getQuant(0)->type == QuantType::EXISTS);
    complete_values.clear();
    for(; qi < formula_->numQuants(); qi += 2)
    {
      const Quant* quant = formula_->getQuant(qi);
      assert(quant->type == QuantType::FORALL);
      for(const_var_iterator v_iter = quant->begin(); v_iter != quant->end(); v_iter++)
      {
        Var v_sub = vars[formula_->getDepth(*v_iter)] + formula_->getLocalPosition(*v_iter);
        complete_values.push_back(solver_b_.getValue(v_sub));
      }
    }
    assignment->update(complete_values);
    assignment->rehash();
    
    if(subformula_solutions_b_.find(assignment) != subformula_solutions_b_.end())
      continue;
  
    const Assignment* exp_assignment = subformula_exps_b_[si];
    formula_->learn_blocking_clause(exp_assignment, assignment);
    
    counter += 1;
    
    extendA(Assignment::copy_assignment(assignment));
  }
  
  assert(counter != 0);
  
  Assignment::destroy_assignment(assignment);
}


void NanoQBF::completeB()
{
  Assignment* assignment = Assignment::make_assignment(formula_->numExistential());
  
  unsigned counter = 0;
  
  for(unsigned si = 0; si < subformula_vars_a_.size(); si++)
  {
    const std::vector<Var>& vars = subformula_vars_a_[si];
    unsigned qi = (unsigned)(formula_->getQuant(0)->type == QuantType::FORALL);
    complete_values.clear();
    for(; qi < formula_->numQuants(); qi += 2)
    {
      const Quant* quant = formula_->getQuant(qi);
      assert(quant->type == QuantType::EXISTS);
      for(const_var_iterator v_iter = quant->begin(); v_iter != quant->end(); v_iter++)
      {
        Var v_sub = vars[formula_->getDepth(*v_iter)] + formula_->getLocalPosition(*v_iter);
        complete_values.push_back(solver_a_.getValue(v_sub));
      }
    }
    assignment->update(complete_values);
    assignment->rehash();
  
    if(subformula_solutions_a_.find(assignment) != subformula_solutions_a_.end())
      continue;
  
    const Assignment* exp_assignment = subformula_exps_a_[si];
    formula_->learn_blocking_cube(assignment, exp_assignment);
    
    counter += 1;
    
    extendB(Assignment::copy_assignment(assignment));
  }
  
  assert(counter != 0);
  
  Assignment::destroy_assignment(assignment);
}


void NanoQBF::extendA(Assignment* assignment)
{
  // LOG("extendA start:\n");
  subformula_solutions_b_.insert(assignment);
  subformula_exps_a_.push_back(assignment);
  
  bool cache_possible = true;
  bool skip = false;
  
  std::vector<Var> subformula_vars;
  Assignment* uni = Assignment::make_assignment(assignment->size);
  unsigned uni_size = 0;
  
  unsigned qi = 0;
  
  if(formula_->getQuant(0)->type == QuantType::EXISTS)
  {
    subformula_vars.push_back(1);
    qi = 1;
  }
  
  for(; qi < formula_->numQuants(); qi++)
  {
    const Quant* quant = formula_->getQuant(qi);
    if(quant->type == QuantType::FORALL)
    {
      skip = false;
      uni_size += quant->size;
  
      // to avoid index computation nightmares
      subformula_vars.push_back(0);
      
      if(!cache_possible) continue;
      
      assignment->make_subassignment(uni, uni_size);
      
      const auto cache_iter = vars_a_.find(uni);
      if(cache_iter == vars_a_.end())
      {
        cache_possible = false;
        continue;
      }
      
      subformula_vars.push_back(cache_iter->second);
      skip = true;
    }
    else if(!skip)
    {
      Var subst = solver_a_.reserveVars(quant->size);
      subformula_vars.push_back(subst);
      assignment->make_subassignment(uni, uni_size);
      vars_a_[Assignment::copy_assignment(uni)] = subst;
    }
  }
  
  Assignment::destroy_assignment(uni);
  subformula_vars_a_.push_back(subformula_vars);
  
  for(unsigned ci = 0; ci < formula_->numClauses(); ci++)
  {
    const Clause* clause = formula_->getClause(ci);
    bool sat = false;
    for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
      sat = sat | (sign(*l_iter) != assignment->get(formula_->getGlobalPosition(var(*l_iter))));
    
    if(sat) continue;
  
    extend_clause.clear();
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
    {
      Var v = var(*l_iter); bool s = sign(*l_iter);
      extend_clause.push_back(make_lit(subformula_vars[formula_->getDepth(v)] + formula_->getLocalPosition(v), s));
    }
    
    solver_a_.addClause(extend_clause);
  }
}

void NanoQBF::extendB(Assignment* assignment)
{
  // LOG("extendB start:\n");
  
  subformula_solutions_a_.insert(assignment);
  subformula_exps_b_.push_back(assignment);
  
  bool cache_possible = true;
  bool skip = false;
  
  std::vector<Var> subformula_vars;
  Assignment* exi = Assignment::make_assignment(assignment->size);
  unsigned exi_size = 0;
  
  unsigned qi = 0;
  
  if(formula_->getQuant(0)->type == QuantType::FORALL)
  {
    subformula_vars.push_back(1);
    qi = 1;
  }
  
  for(; qi < formula_->numQuants(); qi++)
  {
    const Quant* quant = formula_->getQuant(qi);
    if(quant->type == QuantType::EXISTS)
    {
      skip = false;
      exi_size += quant->size;
      
      // to avoid index computation nightmares
      subformula_vars.push_back(0);
      
      if(!cache_possible) continue;
      
      assignment->make_subassignment(exi, exi_size);
      
      const auto cache_iter = vars_b_.find(exi);
      if(cache_iter == vars_b_.end())
      {
        cache_possible = false;
        continue;
      }
      
      subformula_vars.push_back(cache_iter->second);
      skip = true;
    }
    else if(!skip)
    {
      Var subst = solver_b_.reserveVars(quant->size);
      subformula_vars.push_back(subst);
      assignment->make_subassignment(exi, exi_size);
      vars_b_[Assignment::copy_assignment(exi)] = subst;
    }
  }
  
  Assignment::destroy_assignment(exi);
  subformula_vars_b_.push_back(subformula_vars);
  
  for(unsigned ci = 0; ci < formula_->numCubes(); ci++)
  {
    const Clause* clause = formula_->getCube(ci);
    bool sat = false;
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
      sat = sat | (sign(*l_iter) != assignment->get(formula_->getGlobalPosition(var(*l_iter))));
    
    if(sat) continue;
    
    extend_clause.clear();
    for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
    {
      Var v = var(*l_iter); bool s = sign(*l_iter);
      extend_clause.push_back(make_lit(subformula_vars[formula_->getDepth(v)] + formula_->getLocalPosition(v), s));
    }
    
    solver_b_.addClause(extend_clause);
  }
  
  
  std::vector<Lit> global_nand;
  std::vector<Lit> lit_cl;
  lit_cl.push_back(0);
  lit_cl.push_back(0);
  
  for(unsigned ci = 0; ci < formula_->numClauses(); ci++)
  {
    const Clause* clause = formula_->getClause(ci);
    bool sat = false;
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
      sat = sat | (sign(*l_iter) != assignment->get(formula_->getGlobalPosition(var(*l_iter))));
      
    if(sat) continue;
  
    extend_clause.clear();
    
    for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
    {
      Var v = var(*l_iter); bool s = sign(*l_iter);
      extend_clause.push_back(make_lit(subformula_vars[formula_->getDepth(v)] + formula_->getLocalPosition(v), s));
    }
    
    Lit x_i = make_lit(solver_b_.reserveVars(1), true);
    assert(x_i < 0);
    global_nand.push_back(x_i);
    
    lit_cl[0] = -x_i;
    
    for(const Lit l : extend_clause)
    {
      lit_cl[1] = -l;
      solver_b_.addClause(lit_cl);
    }
  }
  solver_b_.addClause(global_nand);
}

