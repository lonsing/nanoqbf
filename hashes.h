//
// Created by vedad on 18/06/18.
//

#ifndef NANOQBF_HASHES_H
#define NANOQBF_HASHES_H

#include "types/Assignment.h"

namespace std
{
  /// Custom hash operator for Assignment objects
  template<> struct hash<Assignment>
  {
    size_t operator()(const Assignment& assignment) const;
  };
  
  /// Custom hash operator for Assignment pointers
  template<> struct hash<Assignment*>
  {
    size_t operator()(const Assignment* assignment) const;
  };
}

/// Operator class for comparing two Assignment objects from pointers
struct CompAssignment
{
  bool operator()(const Assignment* lhs, const Assignment* rhs) const;
};



#endif //NANOQBF_HASHES_H
