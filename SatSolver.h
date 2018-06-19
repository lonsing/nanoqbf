//
// Created by vedad on 18/06/18.
//

#ifndef MINIQBF_SATSOLVER_H
#define MINIQBF_SATSOLVER_H

#include "ipasir.h"
#include "types.h"

#include <vector>

class SatSolver
{
public:
  inline SatSolver() : solver_(ipasir_init()) { }
  inline ~SatSolver() { ipasir_release(solver_); }
  inline bool solve() { return ipasir_solve(solver_) == 10; }
  inline void addClause(std::vector<Lit>& clause)
  {
    for(const Lit l : clause)
      ipasir_add(solver_, l);
    ipasir_add(solver_, 0);
  }
  inline bool getValue(Var v) {return ipasir_val(solver_, v) > 0; }
  
private:
  void* solver_;
};









#endif //MINIQBF_SATSOLVER_H
