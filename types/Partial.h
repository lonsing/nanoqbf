//
// Created by vedad on 25/07/18.
//

#ifndef NANOQBF_ASSUMPTION_H
#define NANOQBF_ASSUMPTION_H

#include <vector>
#include <stddef.h>

class Partial
{
public:
  enum Value{
    NONE = 0,
    TRUE = 1,
    FALSE = 2
  };
  
  inline void resize(size_t size);
  inline Partial::Value get(size_t index) const;
  inline void set(size_t index, Partial::Value val);
  inline size_t size() const;
private:
  std::vector<bool> bits;
};

void Partial::resize(size_t size)
{
  bits.resize(2 * size, false);
}

Partial::Value Partial::get(size_t index) const
{
  return (Partial::Value)(bits[2 * index] + 2 * bits[2 * index + 1]);
}

void Partial::set(size_t index, Partial::Value val)
{
  bits[2 * index] = (int)val % 2;
  bits[2 * index + 1] = (int)val / 2;
}

size_t Partial::size() const
{
  return bits.size() / 2;
}

#endif //NANOQBF_PARTIAL_H
