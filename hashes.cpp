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
}