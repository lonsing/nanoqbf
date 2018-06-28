//
// Created by vedad on 27/06/18.
//

#ifndef MINIQBF_CLAUSE_H
#define MINIQBF_CLAUSE_H

#include "common.h"
#include <iosfwd>
#include <vector>

struct Clause
{
  unsigned int size_a;
  unsigned int size_e;
  
  Lit lits[2];
  
  static Clause* make_clause(std::vector<Lit>& exi, std::vector<Lit>& uni);
  static void destroy_clause(Clause* clause);
  
  inline const_lit_iterator begin_a() const {return lits;}
  inline const_lit_iterator end_a() const {return lits + size_a;}
  
  inline const_lit_iterator begin_e() const {return lits + size_a;}
  inline const_lit_iterator end_e() const {return lits + size_a + size_e;}
  
  friend std::ostream& operator<<(std::ostream& out, const Clause& c);
};


#endif //MINIQBF_CLAUSE_H
