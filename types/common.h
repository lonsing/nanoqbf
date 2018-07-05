//
// Created by vedad on 27/06/18.
//

#ifndef NANOQBF_COMMON_H
#define NANOQBF_COMMON_H

#include <stdlib.h>

typedef int             Lit;
typedef int*            lit_iterator;
typedef const int*      const_lit_iterator;

typedef unsigned        Var;
typedef unsigned*       var_iterator;
typedef const unsigned* const_var_iterator;

inline Var var(Lit l)
{
  return std::abs(l);
}

inline bool sign(Lit l)
{
  return l < 0;
}

inline Lit make_lit(Var v, bool sign)
{
  return (Lit)((v ^ -((Lit)sign)) + (Lit)sign);
}

inline bool lit_order(Lit a, Lit b)
{
  const Var va = var(a);
  const Var vb = var(b);
  return va < vb || (va == vb && a < b);
}

enum QuantType
{
  NONE,
  EXISTS,
  FORALL
};


#endif //NANOQBF_COMMON_H
