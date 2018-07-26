//
// Created by vedad on 17/06/18.
//

#include "Assignment.h"

#include "../Logger.h"
#include <algorithm>
#include <assert.h>
#include <bits/functional_hash.h>
#include <cstring>
#include <new>
#include <iomanip>


Assignment* Assignment::make_assignment(unsigned int size)
{
  unsigned int bits_size = (size + 7) / 8;
  unsigned int bytes = sizeof(Assignment) - sizeof bits + bits_size;
  unsigned int qwords = (bytes + sizeof(size_t) - 1) / sizeof(size_t);
  size_t* ptr = new size_t[qwords];
  assert(((size_t)ptr & 0x7UL) == 0x0UL);

#ifndef NDEBUG
  for (size_t* pi = ptr; pi < ptr + qwords; pi++) *pi = 0;
#endif
    
  Assignment* assignment = (Assignment*)ptr;
  assignment->size = size;
  
  return assignment;
}

Assignment* Assignment::make_assignment(std::vector<Lit>& base)
{
  Assignment* assignment = Assignment::make_assignment((unsigned int)base.size());
  
  assignment->update(base);
  assignment->rehash();
  
  return assignment;
}

void Assignment::make_subassignment(Assignment* assignment, unsigned nsize)
{
  assert(nsize <= size);
  unsigned int nbytes = (nsize + 7) / 8;
  assignment->size = nsize;
  std::memcpy(assignment->bits, bits, nbytes);
  assignment->bits[nbytes - 1] &= (uint8_t)0xff << (nbytes * 8 - nsize);
  assignment->rehash();
}

Assignment* Assignment::copy_assignment(Assignment* original)
{
  Assignment* res = Assignment::make_assignment(original->size);
  unsigned int bytes = sizeof(Assignment) - sizeof bits + (original->size + 7) / 8;
  std::memcpy((char*)res, (char*)original, bytes);
  
  return res;
}

void Assignment::update(std::vector<Lit>& base)
{
  assert(base.size() == size);
  
  auto it = base.begin(); // iter for base
  int shifts = 0;         // shifts left for byte
  int byte_id = 0;        // index of bits field
  while(it != base.end())
  {
    shifts = 8;
    char byte = 0;
    while(shifts > 0 && it != base.end())
    {
      byte = (byte << 1) ^ (*it > 0);
      it++; shifts--;
    }
    if(it == base.end())
      byte <<= shifts;
    bits[byte_id] = byte;
    byte_id++;
  }
}

void Assignment::destroy_assignment(Assignment* assignment)
{
  delete[] (size_t*)assignment;
}

void Assignment::set(unsigned int index, bool value)
{
  assert(index < size);
  assert(value == 0 || value == 1);
  
  unsigned byte_id = index / 8;
  unsigned bit_id = 7 - (index % 8);
  // the following sets the correct bit
  bits[byte_id] ^= (-((char)value) ^ bits[byte_id]) & ((char)0x01 << bit_id);
}

bool Assignment::get(unsigned int index) const
{
  assert(index < size);
  
  unsigned byte_id = index / 8;
  unsigned bit_id = 7 - (index % 8);
  
  return (bool)((bits[byte_id] >> bit_id) & 0x01);
}

bool operator==(const Assignment& lhs, const Assignment& rhs)
{
  return (lhs.size == rhs.size) &&
         (lhs.hash_value == rhs.hash_value) &&
         (std::memcmp(lhs.bits, rhs.bits, (lhs.size + 7) / 8) == 0);
}

std::ostream& operator<<(std::ostream& out, const Assignment& a)
{
  out << "c Assignment " << std::setw(16) << std::setfill('0')
      << std::hex << a.hash_value << " : " << std::dec;
  for (unsigned int i = 0; i < a.size; i++)
    out << a.get(i);
  return out;
}