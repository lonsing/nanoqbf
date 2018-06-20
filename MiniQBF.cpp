//
// Created by vedad on 16/06/18.
//

#include "MiniQBF.h"

MiniQBF::MiniQBF(const Formula* formula) :
formula_(formula)
{
  const Quant* qfirst = formula_->getQuant(0);
  if(qfirst->type == QuantType::EXISTS)
    solver_a_.reserveVars(qfirst->size);
  else
    solver_b_.reserveVars(qfirst->size);
}

MiniQBF::~MiniQBF()
{
  for(const auto & a : subformula_solutions_a_)
    Assignment::destroy_assignment(a);
  for(const auto & a : subformula_solutions_b_)
    Assignment::destroy_assignment(a);
  
  for(const auto & av : vars_a_)
    Assignment::destroy_assignment(av.first);
  for(const auto & av : vars_b_)
    Assignment::destroy_assignment(av.first);
}

int MiniQBF::solve()
{
  int res = initA();
  if(res != 0) return res;
  
  LOG("Entering main loop");
  
  while(true)
  {
    LOG("subformuals A: %d", subformula_vars_a_.size());
    LOG("subformuals B: %d", subformula_vars_b_.size());
    
    if(solver_a_.solve() == 20) return 20;
    completeB();
    if(solver_b_.solve() == 20) return 10;
    completeA();
  }
}

int MiniQBF::initA()
{
  SatSolver warmup_solver;
  for(unsigned ci = 0; ci < formula_->numClauses(); ci++)
    warmup_solver.addClause(formula_->getClause(ci));
  
  unsigned warmup_limit = 1;
  
  std::vector<Lit> values;
  
  for(unsigned wi = 0; wi < warmup_limit; wi++)
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
    Assignment* assignemnt = Assignment::make_assignment(values);
    std::cout << *assignemnt << std::endl;
    subformula_solutions_b_.insert(assignemnt);
    extendA(assignemnt);
    
    for(unsigned vi = 0; vi < values.size(); vi++)
      values[vi] = -values[vi];
    
    warmup_solver.addClause(values);
  }
  return 0;
}

void MiniQBF::completeA()
{
  Assignment* assignment = Assignment::make_assignment(formula_->numUniversal());
  std::vector<Lit> values;
  
  unsigned counter = 0;
  
  for(const std::vector<Var>& vars : subformula_vars_b_)
  {
    unsigned qi = (unsigned)(formula_->getQuant(0)->type == QuantType::EXISTS);
    values.clear();
    for(; qi < formula_->numQuants(); qi += 2)
    {
      const Quant* quant = formula_->getQuant(qi);
      assert(quant->type == QuantType::FORALL);
      for(const_var_iterator v_iter = quant->begin(); v_iter != quant->end(); v_iter++)
      {
        Var v_sub = vars[formula_->getDepth(*v_iter)] + formula_->getLocalPosition(*v_iter);
        values.push_back(solver_b_.getValue(v_sub));
      }
    }
    assignment->update(values);
    assignment->rehash();
    
    if(subformula_solutions_b_.find(assignment) != subformula_solutions_b_.end())
      continue;
    
    counter += 1;
    
    Assignment* a = Assignment::copy_assignment(assignment);
    subformula_solutions_b_.insert(a);
    extendA(a);
  }
  
  assert(counter != 0);
  
  Assignment::destroy_assignment(assignment);
}


void MiniQBF::completeB()
{
  Assignment* assignment = Assignment::make_assignment(formula_->numExistential());
  std::vector<Lit> values;
  
  unsigned counter = 0;
  
  for(const std::vector<Var>& vars : subformula_vars_a_)
  {
    unsigned qi = (unsigned)(formula_->getQuant(0)->type == QuantType::FORALL);
    values.clear();
    for(; qi < formula_->numQuants(); qi += 2)
    {
      const Quant* quant = formula_->getQuant(qi);
      assert(quant->type == QuantType::EXISTS);
      for(const_var_iterator v_iter = quant->begin(); v_iter != quant->end(); v_iter++)
      {
        Var v_sub = vars[formula_->getDepth(*v_iter)] + formula_->getLocalPosition(*v_iter);
        values.push_back(solver_a_.getValue(v_sub));
      }
    }
    assignment->update(values);
    assignment->rehash();
  
    if(subformula_solutions_a_.find(assignment) != subformula_solutions_a_.end())
      continue;
    
    counter += 1;
    
    Assignment* a = Assignment::copy_assignment(assignment);
    subformula_solutions_a_.insert(a);
    extendB(a);
  }
  
  assert(counter != 0);
  
  Assignment::destroy_assignment(assignment);
}


void MiniQBF::extendA(Assignment* assignment)
{
  // LOG("extendA start:\n");
  
  bool cache_possible = true;
  bool skip = false;
  
  std::vector<Var> subformula_vars;
  std::vector<Lit> cl;
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
      sat = sat || (sign(*l_iter) != assignment->get(formula_->getGlobalPosition(var(*l_iter))));
    
    if(sat) continue;
  
    cl.clear();
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
    {
      Var v = var(*l_iter); bool s = sign(*l_iter);
      cl.push_back(make_lit(subformula_vars[formula_->getDepth(v)] + formula_->getLocalPosition(v), s));
    }
    
    solver_a_.addClause(cl);
  }
}

void MiniQBF::extendB(Assignment* assignment)
{
  // LOG("extendB start:\n");
  
  bool cache_possible = true;
  bool skip = false;
  
  std::vector<Var> subformula_vars;
  std::vector<Lit> cl;
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
  
  std::vector<Lit> global_nand;
  std::vector<Lit> lit_cl;
  lit_cl.push_back(0);
  lit_cl.push_back(0);
  
  for(unsigned ci = 0; ci < formula_->numClauses(); ci++)
  {
    const Clause* clause = formula_->getClause(ci);
    bool sat = false;
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
      sat = sat || (sign(*l_iter) != assignment->get(formula_->getGlobalPosition(var(*l_iter))));
      
    if(sat) continue;
  
    cl.clear();
    
    for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
    {
      Var v = var(*l_iter); bool s = sign(*l_iter);
      cl.push_back(make_lit(subformula_vars[formula_->getDepth(v)] + formula_->getLocalPosition(v), s));
    }
    
    Lit x_i = make_lit(solver_b_.reserveVars(1), true);
    assert(x_i < 0);
    global_nand.push_back(x_i);
    
    lit_cl[0] = -x_i;
    
    for(const Lit l : cl)
    {
      lit_cl[1] = -l;
      solver_b_.addClause(lit_cl);
    }
  }
  solver_b_.addClause(global_nand);
}

