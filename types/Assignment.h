//
// Created by vedad on 07/10/17.
//

#ifndef NANOQBF_ASSIGNMENT_H
#define NANOQBF_ASSIGNMENT_H

#include <vector>
#include <iostream>
#include <assert.h>

#include "common.h"

/// Structure representing an assignment of truth values to variables
struct Assignment
{
  size_t hash_value; ///< Hash value used for unordered data structures
  unsigned int size; ///< Valid size of #bits expressed in bits
  char   bits[4];    ///< Vector of bits containing the assigned Boolean values
  
  /// Constructs an empty Assignment of given size in memory
  static Assignment* make_assignment(unsigned int size);
  
  /// Constructs an Assignment from a given literal vector
  static Assignment* make_assignment(std::vector<Lit>& base);
  
  /// Constructs a copy of Assignment \a original
  static Assignment* copy_assignment(Assignment* original);
  
  /// Destroys an Assignment and deallocates memory
  static void destroy_assignment(Assignment* assignment);
  
  /// Resizes Assignment \a assignment to size \a nsize, without deallocating
  void make_subassignment(Assignment* assignment, unsigned nsize);
  
  /// Sets bit at \a index to \a value
  void set(unsigned int index, bool value);
  
  /// Gets value of bit at \a index
  bool get(unsigned int index) const;
  
  /// Updates all assigned bits according to base, without rehashing
  void update(std::vector<Lit>& base);
  
  /// Rehashes the object, must be called after modifying the object
  inline void rehash();
  
  /// Compares two Assignment objects
  friend bool operator==(const Assignment& lhs, const Assignment& rhs);
  
  /// Prints Assignment \a a to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Assignment& a);
};

inline void Assignment::rehash()
{
  unsigned int bits_size = (size + 7) / 8;
  hash_value = std::_Hash_impl::hash(&(size), sizeof size + bits_size);
}

struct AssignmentIndex
{
  Assignment* assignment;
  int index;
  inline AssignmentIndex(Assignment* a, int i);
};

inline AssignmentIndex::AssignmentIndex(Assignment* a, int i) : assignment(a), index(i) {}

#endif // NANOQBF_ASSIGNMENT_H
