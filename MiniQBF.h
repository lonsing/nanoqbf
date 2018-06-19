//
// Created by vedad on 16/06/18.
//

#ifndef MINIQBF_MINIQBF_H
#define MINIQBF_MINIQBF_H

#include <unordered_map>
#include "SatSolver.h"
#include "types.h"
#include "hashes.h"


class MiniQBF
{
public:
private:
  SatSolver solver_a_;
  SatSolver solver_b_;
  
  std::unordered_map<Assignment, Var> vars_a_;
  std::unordered_map<Assignment, Var> vars_b_;
  
  std::vector<std::vector<Var>> subformula_vars_a_;
  std::vector<std::vector<Var>> subformula_vars_b_;
  
};


#endif // MINIQBF_MINIQBF_H
