//
// Created by vedad on 25/07/18.
//

#ifndef NANOQBF_ASSUMPTION_H
#define NANOQBF_ASSUMPTION_H

#include <vector>
#include <stddef.h>

class Assumption
{
public:
  enum Value{
    NONE = 0,
    TRUE = 1,
    FALSE = 2
  };
  
  inline void resize(size_t size);
  inline Assumption::Value get(size_t index) const;
  inline void set(size_t index, Assumption::Value val);
  inline size_t size() const;
private:
  std::vector<bool> bits;
};

void Assumption::resize(size_t size)
{
  bits.resize(2 * size, false);
}

Assumption::Value Assumption::get(size_t index) const
{
  return (Assumption::Value)(bits[2 * index] + 2 * bits[2 * index + 1]);
}

void Assumption::set(size_t index, Assumption::Value val)
{
  bits[2 * index] = (int)val % 2;
  bits[2 * index + 1] = (int)val / 2;
}

size_t Assumption::size() const
{
  return bits.size() / 2;
}

#endif //NANOQBF_ASSUMPTION_H
