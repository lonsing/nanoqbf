//
// Created by vedad on 18/06/18.
//

#ifndef NANOQBF_HASHES_H
#define NANOQBF_HASHES_H

#include "types/Assignment.h"

namespace std
{
  template<> struct hash<Assignment>
  {
    size_t operator()(const Assignment& assignment) const;
  };
  
  template<> struct hash<Assignment*>
  {
    size_t operator()(const Assignment* assignment) const;
  };
}

struct CompAssignment
{
  bool operator()(const Assignment* lhs, const Assignment* rhs) const;
};



#endif //NANOQBF_HASHES_H
