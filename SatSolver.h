//
// Created by vedad on 18/06/18.
//

#ifndef MINIQBF_SATSOLVER_H
#define MINIQBF_SATSOLVER_H

#include "ipasir.h"
#include "types.h"
#include "Logger.h"

#include <vector>

class SatSolver
{
public:
  inline SatSolver() : solver_(ipasir_init()), num_vars(0) { }
  inline ~SatSolver() { ipasir_release(solver_); }
  inline int solve() { return ipasir_solve(solver_); }
  
  inline void addClause(std::vector<Lit>& clause)
  {
    for(const Lit l : clause)
      ipasir_add(solver_, l);
    ipasir_add(solver_, 0);
  }
  
  inline void addClause(const Clause* clause)
  {
    for(const_lit_iterator l_iter = clause->begin_e(); l_iter != clause->end_e(); l_iter++)
      ipasir_add(solver_, *l_iter);
    for(const_lit_iterator l_iter = clause->begin_a(); l_iter != clause->end_a(); l_iter++)
      ipasir_add(solver_, *l_iter);
    ipasir_add(solver_, 0);
  }
  
  inline Lit getValue(Var v) { return ipasir_val(solver_, v); }
  
  inline Var reserveVars(unsigned num)
  {
    Var res = num_vars + 1;
    num_vars += num;
    return res;
  }
  
private:
  void* solver_;
  unsigned num_vars;
};









#endif //MINIQBF_SATSOLVER_H
