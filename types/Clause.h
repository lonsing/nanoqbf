//
// Created by vedad on 27/06/18.
//

#ifndef MINIQBF_CLAUSE_H
#define MINIQBF_CLAUSE_H

#include "common.h"
#include <iosfwd>
#include <vector>

/// Structure representing a clause of a CNF
struct Clause
{
  unsigned int size_a; ///< Allocated size for universal literals in #lits
  unsigned int size_e; ///< Allocated size for existential literals in #lits
  
  Lit lits[2]; ///< Array of literals in the calsue
  
  /// Constructs a clause from a existential and universal literal vectors \a exi and \a uni
  static Clause* make_clause(std::vector<Lit>& exi, std::vector<Lit>& uni);
  
  /// Destroys a clause and deallocates memory
  static void destroy_clause(Clause* clause);
  
  /// Iterator pointing to first universal literal
  inline const_lit_iterator begin_a() const {return lits;}
  
  /// Iterator pointing after last universal literal
  inline const_lit_iterator end_a() const {return lits + size_a;}
  
  /// Iterator pointing to first existential literal
  inline const_lit_iterator begin_e() const {return lits + size_a;}
  
  /// Iterator pointing after last existential literal
  inline const_lit_iterator end_e() const {return lits + size_a + size_e;}
  
  /// Prints Clause \a c to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Clause& c);
};


#endif //MINIQBF_CLAUSE_H
