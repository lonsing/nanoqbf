//
// Created by vedad on 27/06/18.
//

#include "Quant.h"
#include <iostream>
#include <assert.h>

Quant* Quant::make_quant(QuantType qtype, std::vector<Var>& variables)
{
  size_t bytes = sizeof (Quant) + (variables.size() - 2) * sizeof (Var);
  assert(bytes % sizeof(unsigned int) == 0);
  unsigned int* ptr = new unsigned int[bytes / sizeof(unsigned int)];
  
  assert(((size_t)ptr & 0x3UL) == 0x0UL);
  Quant* quant = (Quant*) ptr;
  
  quant->type = qtype;
  quant->size = (unsigned int)variables.size();
  
  for(int i = 0; i < quant->size; i++)
    quant->vars[i] = variables[i];
  
  return quant;
}

void Quant::destroy_quant(Quant* quant)
{
  delete[] (unsigned int*)quant;
}

std::ostream& operator<<(std::ostream& out, const Quant& q)
{
  out << (q.type == QuantType::EXISTS ? "exists" : "forall");
  for (const_var_iterator v_iter = q.begin(); v_iter != q.end(); v_iter++)
    out << " " << *v_iter;
  return out;
}