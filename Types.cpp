//
// Created by vedad on 17/06/18.
//

#include "Types.h"
#include <algorithm>
#include <assert.h>

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
  alignas(8) char * ptr = new char[bytes];
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
  alignas(8) char * ptr = new char[bytes];
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