//
// Created by vedad on 16/06/18.
//

#ifndef NANOQBF_NANOQBF_H
#define NANOQBF_NANOQBF_H

#include <unordered_map>
#include <unordered_set>
#include "SatSolver.h"
#include "types/Assignment.h"
#include "hashes.h"
#include "Options.h"

class Formula;

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
  void pruneA();
  
  void completeA();
  void completeB();
  std::vector<Lit> complete_values;
  
  void extendA(Assignment* assignment);
  void extendB(Assignment* assignment);
  std::vector<Lit> extend_clause;
};


#endif // NANOQBF_NANOQBF_H
