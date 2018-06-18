//
// Created by vedad on 17/06/18.
//

#include "types.h"
#include <algorithm>
#include <assert.h>
#include <bits/functional_hash.h>
#include <cstring>

std::ostream& operator<<(std::ostream& out, const Quant& q)
{
  out << (q.type == QuantType::EXISTS ? "exists" : "forall");
  for (const_var_iterator v_iter = q.begin(); v_iter != q.end(); v_iter++)
    out << " " << *v_iter;
  return out;
}

std::ostream& operator<<(std::ostream& out, const Clause& c)
{
  for (const_lit_iterator l_iter = c.begin_e(); l_iter != c.end_e(); l_iter++)
    out << *l_iter << " ";
  for (const_lit_iterator l_iter = c.begin_a(); l_iter != c.end_a(); l_iter++)
    out << *l_iter << " ";
  return out;
}

Assignment* Assignment::make_assignemnt(std::vector<Lit>& base)
{
  unsigned int bytes = sizeof(Assignment) - 3 + (base.size() / 8);
  alignas(8) char* ptr = new char[bytes];
  Assignment* a = (Assignment*)ptr;
  a->size = (unsigned int)base.size();
  
  auto it = base.begin(); // iter for base
  int shifts = 0;         // shifts left for byte
  int byte_id = 0;        // index of bits field
  while(it != base.end())
  {
    shifts = 8;
    char byte = 0;
    while(it != base.end() && shifts > 0)
    {
      byte = (byte << 1) ^ (*it > 0);
      it++; shifts--;
    }
    if(it == base.end())
      byte <<= shifts;
    a->bits[byte_id] = byte;
    byte_id++;
  }
  
  a->hash_value = std::_Hash_impl::hash(a->bits, (a->size / 8) + 1);
  
  return a;
}

void Assignment::set(int index, bool value)
{
  assert(index >= 0 && index < size);
  assert(value == 0 || value == 1);
  
  unsigned byte_id = (unsigned)index / 8;
  unsigned bit_id = 7 - (unsigned)index % 8;
  // the following sets the correct bit
  bits[byte_id] ^= (-((char)value) ^ bits[byte_id]) & ((char)0x01 << bit_id);
}

bool Assignment::get(int index)
{
  assert(index >= 0 && index < size);
  
  unsigned byte_id = (unsigned)index / 8;
  unsigned bit_id = 7 - (unsigned)index % 8;
  
  return (bool)((bits[byte_id] >> bit_id) & 0x01);
}

void Assignment::update()
{
  hash_value = std::_Hash_impl::hash(bits, (size / 8) + 1);
}

bool operator==(const Assignment& lhs, const Assignment& rhs)
{
  return (lhs.size == rhs.size) &&
         (lhs.hash_value == rhs.hash_value) &&
         std::memcmp(lhs.bits, rhs.bits, lhs.size / 8 + 1) == 0;
}

Formula::~Formula()
{
  for (Quant* q : prefix)
    delete[] (char*)q;
  for (Clause* c : matrix)
    delete[] (char*)c;
}

void Formula::addClause(std::vector<Lit>& c)
{
  { // scope used for clause simplification
    std::sort(c.begin(), c.end(), lit_order);
    bool tautology = false;
    std::vector<Lit>::const_iterator i = c.begin();
    std::vector<Lit>::iterator j = c.begin();
    Lit prev = 0;
    while (i != c.end())
    {
      Lit lit = *i++;
      if (lit == -prev)
      {
        tautology = true;
        break;
      }
      if (lit !=  prev) *j++ = prev = lit;
    }
    
    if (tautology) return;
    
    if (j != c.end())
      c.resize(j - c.begin());
  }
  
  tmp_exists.clear();
  tmp_forall.clear();
  
  for (const Lit l : c)
  {
    const Var v = var(l);
    if (!isQuantified(v))
      addFreeVar(v);
    
    if (isExistential(v))
      tmp_exists.push_back(l);
    else
      tmp_forall.push_back(l);
  }
  
  size_t bytes = sizeof (Clause) + (tmp_exists.size() + tmp_forall.size() - 2) * sizeof (Lit);
  alignas(8) char* ptr = new char[bytes];
  Clause* clause = (Clause*) ptr;
  
  clause->size_e = (unsigned int)tmp_exists.size();
  clause->size_a = (unsigned int)tmp_forall.size();
  
  for(int li = 0; li < clause->size_e; li++)
    clause->lits[li] = tmp_exists[li];
  for(int li = 0; li < clause->size_a; li++)
    clause->lits[clause->size_e + li] = tmp_forall[li];
  
  matrix.push_back(clause);
}

int Formula::addQuantifier(QuantType type, std::vector<Var>& variables)
{
  if(type == QuantType::NONE)
    type = QuantType::EXISTS;
  
  for (const Var v : variables)
  {
    if (quantify(v, type))
    {
      printf("Variable %d is already quantified", v);
      return -1;
    }
  }
  
  if (prefix.empty() && type == QuantType::EXISTS)
  {
    for (const Var v : variables)
      free_variables.push_back(v);
    return 0;
  }
  
  size_t bytes = sizeof (Quant) + (variables.size() - 2) * sizeof (Var);
  alignas(8) char* ptr = new char[bytes];
  Quant* quant= (Quant*) ptr;
  
  quant->type = type;
  quant->size = (unsigned int)variables.size();
  
  for(int i = 0; i < quant->size; i++)
    quant->vars[i] = variables[i];
  
  prefix.push_back(quant);
  
  return 0;
}

void Formula::addFreeVar(Var v)
{
  assert(!isQuantified(v));
  free_variables.push_back(v);
  quantify(v, QuantType::EXISTS);
}

int Formula::quantify(const Var v, const QuantType type)
{
  if(v >= is_quantified.size())
  {
    is_quantified.resize(v + 1, false);
    is_existential.resize(v + 1, false);
  }
  
  if(is_quantified[v]) return -1;
  
  is_quantified[v] = true;
  is_existential[v] = (type == QuantType::EXISTS);
  
  return 0;
}

std::ostream& operator<<(std::ostream& out, const Formula& f)
{
  out << "exists";
  for (const Var v : f.free_variables)
    out << " " << v;
  out << std::endl;
  for (const Quant* q : f.prefix)
    out << *q << std::endl;
  for (const Clause* c : f.matrix)
    out << *c << std::endl;
  return out;
}