//
// Created by vedad on 17/06/18.
//

#include "types.h"
#include <algorithm>
#include <assert.h>
#include <bits/functional_hash.h>
#include <cstring>
#include <new>

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

Quant* Quant::make_quant(QuantType qtype, std::vector<Var>& vars)
{
  size_t bytes = sizeof (Quant) + (vars.size() - 2) * sizeof (Var);
  char* ptr = new(std::align_val_t(4)) char[bytes];
  assert(((size_t)ptr & 0x3UL) == 0x0UL);
  Quant* quant = (Quant*) ptr;
  
  quant->type = qtype;
  quant->size = (unsigned int)vars.size();
  
  for(int i = 0; i < quant->size; i++)
    quant->vars[i] = vars[i];
}

void Quant::destroy_quant(Quant* quant)
{
  ::operator delete[]((char*)quant, std::align_val_t(4));
}

Clause* Clause::make_clause(std::vector<Lit>& exi, std::vector<Lit>& uni)
{
  unsigned int bytes = sizeof (Clause) + (exi.size() + uni.size() - 2) * sizeof (Lit);
  char* ptr = new(std::align_val_t(4)) char[bytes];
  assert(((size_t)ptr & 0x3UL) == 0x0UL);
  Clause* clause = (Clause*) ptr;
  
  clause->size_e = (unsigned int)exi.size();
  clause->size_a = (unsigned int)uni.size();
  
  for(int i = 0; i < clause->size_e; i++)
    clause->lits[i] = exi[i];
  
  for(int i = 0; i < clause->size_a; i++)
    clause->lits[clause->size_e + i] = uni[i];
  
  return clause;
}

void Clause::destroy_clause(Clause* clause)
{
  ::operator delete[]((char*)clause, std::align_val_t(4));
}

Assignment* Assignment::make_assignment(std::vector<Lit>& base)
{
  unsigned int bits_size = (unsigned int)(base.size() + 7) / 8;
  unsigned int bytes = sizeof(Assignment) - sizeof bits + bits_size;
  char* ptr = new(std::align_val_t(8)) char[bytes];
  assert(((size_t)ptr & 0x7UL) == 0x0UL);
  
  Assignment* assignment = (Assignment*)ptr;
  assignment->size = (unsigned int)base.size();
  
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
    assignment->bits[byte_id] = byte;
    byte_id++;
  }
  
  assignment->hash_value = std::_Hash_impl::hash(&(assignment->size), sizeof size + bits_size);
  
  return assignment;
}

void Assignment::destroy_assignment(Assignment* assignment)
{
  ::operator delete[]((char*)assignment, std::align_val_t(8));
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
  unsigned int bits_size = (size + 7) / 8;
  hash_value = std::_Hash_impl::hash(&(size), sizeof size + bits_size);
}

bool operator==(const Assignment& lhs, const Assignment& rhs)
{
  return (lhs.size == rhs.size) &&
         (lhs.hash_value == rhs.hash_value) &&
         std::memcmp(lhs.bits, rhs.bits, (unsigned int)(lhs.size + 7) / 8) == 0;
}

Formula::~Formula()
{
  for (Quant* q : prefix)
    Quant::destroy_quant(q);
  for (Clause* c : matrix)
    Clause::destroy_clause(c);
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
  
  Clause* clause = Clause::make_clause(tmp_exists, tmp_forall);
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
  
  Quant* quant = Quant::make_quant(type, variables);
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