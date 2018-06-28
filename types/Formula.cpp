//
// Created by vedad on 27/06/18.
//

#include "Formula.h"

#include "Clause.h"
#include <algorithm>
#include <iostream>

Formula::~Formula()
{
  for (Quant* q : prefix)
    Quant::destroy_quant(q);
  for (Clause* c : matrix)
    Clause::destroy_clause(c);
}

Formula::Formula()
{
  prefix.push_back(nullptr);
  position_counters.push_back(0);
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
  
  if (prefix.size() == 1 && type == QuantType::EXISTS)
  {
    for (const Var v : variables)
    {
      free_variables.push_back(v);
      if (quantify(v, 0))
      {
        printf("Variable %d is already quantified", v);
        return -1;
      }
    }
    return 0;
  }
  
  position_counters.push_back(0);
  
  for (const Var v : variables)
  {
    if (quantify(v, (unsigned int)prefix.size()))
    {
      printf("Variable %d is already quantified", v);
      return -1;
    }
  }
  
  Quant* quant = Quant::make_quant(type, variables);
  prefix.push_back(quant);
  
  return 0;
}

void Formula::finalise()
{
  if(free_variables.empty())
  {
    prefix.erase(prefix.begin());
    position_counters.erase(position_counters.begin());
    for(unsigned i = 0; i < quant_depth.size(); i++)
      if(quant_depth[i] > 0)
        quant_depth[i]-= 1;
  }
  else
  {
    prefix[0] = Quant::make_quant(QuantType::EXISTS, free_variables);
  }
  
  position_offset.resize(position_counters.size(), 0);
  
  num_exists = 0;
  num_forall = 0;
  
  for(const Quant* q : prefix)
    if(q->type == QuantType::EXISTS)
      num_exists += q->size;
    else
      num_forall += q->size;
  
  for(unsigned qi = 2; qi < prefix.size(); qi++)
    position_offset[qi] = position_offset[qi - 2] + prefix[qi-2]->size;
}


void Formula::addFreeVar(Var v)
{
  assert(!isQuantified(v));
  free_variables.push_back(v);
  quantify(v, 0);
}

int Formula::quantify(const Var v, unsigned depth)
{
  if(v >= quant_depth.size())
  {
    quant_depth.resize(v + 1, -1);
    quant_position.resize(v + 1, 0);
  }
  
  if(quant_depth[v] != -1) return -1;
  
  quant_depth[v] = depth;
  
  quant_position[v] = position_counters[depth]++;
  
  return 0;
}

std::ostream& operator<<(std::ostream& out, const Formula& f)
{
  for (const Quant* q : f.prefix)
    out << *q << std::endl;
  for (const Clause* c : f.matrix)
    out << *c << std::endl;
  return out;
}