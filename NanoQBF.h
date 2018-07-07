//
// Created by vedad on 16/06/18.
//

#ifndef NANOQBF_NANOQBF_H
#define NANOQBF_NANOQBF_H

#include <unordered_map>
#include <unordered_set>
#include "SatSolver.h"
#include "types/Assignment.h"
#include "types/Formula.h"
#include "hashes.h"
#include "Options.h"
#include "auxutils.h"

class NanoQBF
{
public:
  NanoQBF(const Formula* formula, const Options* options);
  ~NanoQBF();
  int solve();
private:
  const Formula* formula_;
  const Options* options_;
  
  SatSolver solver_a_;
  SatSolver solver_b_;
  
  unsigned iteration_;
  
  std::unordered_set<Assignment*, std::hash<Assignment*>, CompAssignment> subformula_solutions_a_;
  std::unordered_set<Assignment*, std::hash<Assignment*>, CompAssignment> subformula_solutions_b_;
  
  std::unordered_map<Assignment*, Var, std::hash<Assignment*>, CompAssignment> vars_a_;
  std::unordered_map<Assignment*, Var, std::hash<Assignment*>, CompAssignment> vars_b_;
  
  std::vector<std::vector<Var>> subformula_vars_a_;
  std::vector<std::vector<Var>> subformula_vars_b_;
  
  int initA();
  
  inline void pruneA();
  inline void pruneB();
  
  inline void pruneCheckA();
  inline void pruneCheckB();
  
  
  void completeA();
  void completeB();
  std::vector<Lit> complete_values;
  
  void extendA(Assignment* assignment);
  void extendB(Assignment* assignment);
  std::vector<Lit> extend_clause;
};

////// INLINE FUNCTION DEFINITIONS //////
void NanoQBF::pruneA()
{
  printf("c Pruning formula A\n");
  for(const auto & av : vars_a_)
    Assignment::destroy_assignment(av.first);
  vars_a_.clear();
  subformula_vars_a_.clear();
  subformula_solutions_b_.clear();
  solver_a_.reset();
  const Quant* q = formula_->getQuant(0);
  if(q->type == QuantType::EXISTS)
    solver_a_.reserveVars(q->size);
}

void NanoQBF::pruneB()
{
  printf("c Pruning formula B\n");
  for(const auto & av : vars_b_)
    Assignment::destroy_assignment(av.first);
  vars_b_.clear();
  subformula_vars_b_.clear();
  subformula_solutions_a_.clear();
  solver_b_.reset();
  const Quant* q = formula_->getQuant(0);
  if(q->type == QuantType::FORALL)
    solver_b_.reserveVars(q->size);
}

void NanoQBF::pruneCheckA()
{
  bool prune_periodic = (options_->pruning_a == Options::PruningMode::PERIODIC &&
                         iteration_ % options_->pparams_a.period == 0);
  bool prune_dynamic = (options_->pruning_a == Options::PruningMode::DYNAMIC &&
                        read_mem_usage() > options_->memory_limit * 0.8);
  
  if(prune_periodic || prune_dynamic) pruneA();
}

void NanoQBF::pruneCheckB()
{
  bool prune_periodic = (options_->pruning_b == Options::PruningMode::PERIODIC &&
                         iteration_ % options_->pparams_b.period == 0);
  bool prune_dynamic = (options_->pruning_b == Options::PruningMode::DYNAMIC &&
                        read_mem_usage() > options_->memory_limit * 0.8);
  
  if(prune_periodic || prune_dynamic) pruneB();
}










#endif // NANOQBF_NANOQBF_H
