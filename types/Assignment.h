//
// Created by vedad on 07/10/17.
//

#ifndef NANOQBF_ASSIGNMENT_H
#define NANOQBF_ASSIGNMENT_H

#include <vector>
#include <iostream>
#include <assert.h>

#include "common.h"

struct Assignment
{
  size_t hash_value;
  unsigned int size;
  char   bits[4];
  
  static Assignment* make_assignment(std::vector<Lit>& base);
  static Assignment* make_assignment(unsigned int size);
  static Assignment* copy_assignment(Assignment* original);
  static void destroy_assignment(Assignment* assignment);
  
  void make_subassignment(Assignment* assignment, unsigned nsize);
  
  void set(unsigned int index, bool value);
  bool get(unsigned int index) const;
  void update(std::vector<Lit>& base);
  // must be called after modifying the object
  inline void rehash();
  
  friend bool operator==(const Assignment& lhs, const Assignment& rhs);
  friend std::ostream& operator<<(std::ostream& out, const Assignment& q);
};

inline void Assignment::rehash()
{
  unsigned int bits_size = (size + 7) / 8;
  hash_value = std::_Hash_impl::hash(&(size), sizeof size + bits_size);
}


#endif // NANOQBF_ASSIGNMENT_H
