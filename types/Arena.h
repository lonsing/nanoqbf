//
// Created by vedad on 30/07/18.
//

#ifndef NANOQBF_ARENA_H
#define NANOQBF_ARENA_H

#include "Clause.h"
#include <assert.h>

class Arena
{
public:
  inline Arena(unsigned long s);
  inline ~Arena();
  inline Clause* addClause(Clause* clause);
  inline bool full() const;
private:
  int* memory;
  unsigned long size;
  unsigned long top;
};

Arena::Arena(unsigned long s) :
memory(0),
size(s),
top(0)
{
  memory = new int[size];
}

Arena::~Arena()
{
  delete[] memory;
}

inline Clause* Arena::addClause(Clause* clause)
{
  size_t s = clause->alloc_size();
  assert(top + s <= size);
  std::copy((int*)clause, (int*)clause + s, memory + top);
  top += s;
  return (Clause*)(memory + top - s);
}

inline bool Arena::full() const
{
  return top == size;
}

#endif //NANOQBF_ARENA_H
