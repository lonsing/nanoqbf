//
// Created by vedad on 27/06/18.
//

#ifndef NANOQBF_QUANT_H
#define NANOQBF_QUANT_H

#include <iosfwd>
#include <vector>
#include "common.h"

/// Structure representing a quantified vector of variables
struct Quant
{
  QuantType    type : 2;  ///< Type of the quantifier
  unsigned int size : 30; ///< Allocated size of #vars
  Var vars[2];            ///< Array of quantified variables
  
  /// Constructs a quantifier from a quantifier type and variable vector
  static Quant* make_quant(QuantType qtype, std::vector<Var>& variables);
  
  /// Destroys a quantifier and deallocates memory
  static void destroy_quant(Quant* quant);
  
  /// Iterator pointing to first variable
  const_var_iterator begin() const {return vars;}
  
  /// Iterator pointing after last variable
  const_var_iterator end() const {return vars + size;}
  
  /// Prints Quant \a q to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Quant& q);
};


#endif //NANOQBF_QUANT_H
