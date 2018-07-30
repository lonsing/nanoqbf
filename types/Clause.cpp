//
// Created by vedad on 27/06/18.
//

#include "Clause.h"
#include <assert.h>
#include <iostream>

Clause* Clause::make_clause(std::vector<Lit>& exi, std::vector<Lit>& uni)
{
  unsigned int bytes = sizeof (Clause) + (exi.size() + uni.size() - 2) * sizeof (Lit);
  assert(bytes % sizeof(int) == 0);
  int* ptr = new int[bytes / sizeof(int)];
  assert(((size_t)ptr & 0x3UL) == 0x0UL);
  Clause* clause = (Clause*) ptr;
  
  clause->size_a = (unsigned int)uni.size();
  clause->size_e = (unsigned int)exi.size();
  clause->depth = (unsigned)-1;
  
  for(unsigned int i = 0; i < clause->size_a; i++)
    clause->lits[i] = uni[i];
  
  for(unsigned int i = 0; i < clause->size_e; i++)
    clause->lits[clause->size_a + i] = exi[i];
  
  return clause;
}

void Clause::destroy_clause(Clause* clause)
{
  delete[] (int*)clause;
}

std::ostream& operator<<(std::ostream& out, const Clause& c)
{
  if(c.size_e > 0)
    out << "e ";
  for (const_lit_iterator l_iter = c.begin_e(); l_iter != c.end_e(); l_iter++)
    out << *l_iter << " ";
  if(c.size_a)
    out << "a ";
  for (const_lit_iterator l_iter = c.begin_a(); l_iter != c.end_a(); l_iter++)
    out << *l_iter << " ";
  return out;
}