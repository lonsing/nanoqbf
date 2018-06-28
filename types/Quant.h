//
// Created by vedad on 27/06/18.
//

#ifndef MINIQBF_QUANT_H
#define MINIQBF_QUANT_H

#include <iosfwd>
#include <vector>
#include "common.h"


struct Quant
{
  QuantType    type : 2;
  unsigned int size : 30;
  Var vars[2];
  
  static Quant* make_quant(QuantType qtype, std::vector<Var>& variables);
  static void destroy_quant(Quant* quant);
  
  const_var_iterator begin() const {return vars;}
  const_var_iterator end() const {return vars + size;}
  friend std::ostream& operator<<(std::ostream& out, const Quant& q);
};


#endif //MINIQBF_QUANT_H
