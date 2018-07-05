//
// Created by vedad on 18/06/18.
//

#ifndef NANOQBF_SATSOLVER_H
#define NANOQBF_SATSOLVER_H

#include "ipasir.h"
#include "types/Assignment.h"
#include "Logger.h"
#include "types/Clause.h"
#include <vector>

class SatSolver
{
public:
  inline SatSolver() : solver_(ipasir_init()), num_vars(0) { }
  inline ~SatSolver() { ipasir_release(solver_); }
  inline int solve() { return ipasir_solve(solver_); }
  inline void addClause(std::vector<Lit>& clause);
  inline void addClause(const Clause* clause);
  inline Lit getValue(Var v);
  inline Var reserveVars(unsigned num);
  
private:
  void* solver_;
  unsigned num_vars;
};

inline void SatSolver::addClause(std::vector<Lit>& clause)
{
  for(const Lit l : clause)
    ipasir_add(solver_, l);
  ipasir_add(solver_, 0);
}

inline void SatSolver::addClause(const Clause* clause)
{
  for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
    ipasir_add(solver_, *l_iter);
  for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
    ipasir_add(solver_, *l_iter);
  ipasir_add(solver_, 0);
}

inline Lit SatSolver::getValue(Var v)
{
  return ipasir_val(solver_, v);
}

inline Var SatSolver::reserveVars(unsigned num)
{
  Var res = num_vars + 1;
  num_vars += num;
  return res;
}

#endif //NANOQBF_SATSOLVER_H
