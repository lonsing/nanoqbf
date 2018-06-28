//
// Created by vedad on 18/06/18.
//

#ifndef MINIQBF_HASHES_H
#define MINIQBF_HASHES_H

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



#endif //MINIQBF_HASHES_H
