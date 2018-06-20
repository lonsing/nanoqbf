//
// Created by vedad on 18/06/18.
//

#include "hashes.h"

namespace std
{
  size_t hash<Assignment>::operator()(const Assignment& assignment) const
  {
    return assignment.hash_value;
  }
  
  size_t hash<Assignment*>::operator()(const Assignment* assignment) const
  {
    return assignment->hash_value;
  }
}

bool CompAssignment::operator()(const Assignment* lhs, const Assignment* rhs) const
{
  return *lhs == *rhs;
}
