//
// Created by vedad on 27/06/18.
//

#ifndef NANOQBF_CLAUSE_H
#define NANOQBF_CLAUSE_H

#include "common.h"
#include <iosfwd>
#include <vector>

/// Structure representing a clause of a CNF
struct Clause
{
  unsigned int size_a; ///< Allocated size for universal literals in #lits
  unsigned int size_e; ///< Allocated size for existential literals in #lits
  unsigned int depth;  ///< Deepest quantified literal in this clause
  
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
  
  /// Orders clauses by their depth
  static inline bool depth_order(const Clause* a, const Clause* b);
  
  inline size_t alloc_size() const;
  
  /// Prints Clause \a c to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Clause& c);
};

bool Clause::depth_order(const Clause* a, const Clause* b)
{
  return a->depth < b->depth;
}

size_t Clause::alloc_size() const
{
  return ((sizeof (Clause) + (size_e + size_a - 2) * sizeof (Lit)) / sizeof(int));
}


#endif //NANOQBF_CLAUSE_H
