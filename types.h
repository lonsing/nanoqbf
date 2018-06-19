//
// Created by vedad on 07/10/17.
//

#ifndef MINIQBF_TYPES_H
#define MINIQBF_TYPES_H

#include <vector>
#include <iostream>
#include <bitset>
#include <assert.h>

typedef int             Lit;
typedef int*            lit_iterator;
typedef const int*      const_lit_iterator;

typedef unsigned        Var;
typedef unsigned*       var_iterator;
typedef const unsigned* const_var_iterator;

inline Var var(Lit l)
{
  return std::abs(l);
}

inline bool sign(Lit l)
{
  return l < 0;
}

inline bool lit_order(Lit a, Lit b)
{
  const Var va = var(a);
  const Var vb = var(b);
  return va < vb || (va == vb && a < b);
}

enum QuantType
{
  NONE,
  EXISTS,
  FORALL
};

struct Quant
{
  QuantType    type : 2;
  unsigned int size : 30;
  Var vars[2];
  
  static Quant* make_quant(QuantType qtype, std::vector<Var>& variables);
  static void destroy_quant(Quant* quant);
  
  const_var_iterator begin() const {return vars;}
  const_var_iterator end() const {return vars + size;}
};

struct Clause
{
  unsigned int size_e;
  unsigned int size_a;
  Lit lits[2];
  
  static Clause* make_clause(std::vector<Lit>& exi, std::vector<Lit>& uni);
  static void destroy_clause(Clause* clause);
  
  const_lit_iterator begin_e() const {return lits;}
  const_lit_iterator end_e() const {return lits + size_e;}
  const_lit_iterator begin_a() const {return lits + size_e;}
  const_lit_iterator end_a() const {return lits + size_e + size_a;}
};

struct Assignment
{
  size_t hash_value;
  unsigned int size;
  char   bits[4];
  
  static Assignment* make_assignment(std::vector<Lit>& base);
  static void destroy_assignment(Assignment* assignment);
  void set(unsigned int index, bool value);
  bool get(unsigned int index);
  
  void update(); // must be called after modifying the object
  friend bool operator==(const Assignment& lhs, const Assignment& rhs);
};

class Formula
{
public:
  Formula();
  ~Formula();
  
  inline bool isQuantified(Var v)
  {
    return (v < quant_depth.size()) && (quant_depth[v] != -1);
  }
  
  inline bool isExistential(Var v)
  {
    assert(isQuantified(v));
    return prefix[quant_depth[v]] == nullptr ||
    prefix[quant_depth[v]]->type == QuantType::EXISTS;
  }
  
  void addClause(std::vector<Lit>& c);
  void addFreeVar(Var v);
  int addQuantifier(QuantType type, std::vector<Var>& variables);
  void finalise();
  
  inline unsigned long numVars()
  {
    return quant_depth.size() - 1;
  }
  
  inline unsigned long numClauses()
  {
    return matrix.size();
  }
  
  friend std::ostream& operator<<(std::ostream& out, const Formula& f);
  
private:
  
  std::vector<Quant*> prefix;
  std::vector<Clause*> matrix;
  // std::vector<bool> is_quantified;
  // std::vector<bool> is_existential;
  std::vector<int> quant_depth;
  std::vector<unsigned> quant_position;
  std::vector<unsigned> position_counters;
  
  std::vector<Var> free_variables;
  std::vector<Lit> tmp_exists;
  std::vector<Lit> tmp_forall;
  
  int quantify(const Var v, unsigned depth);
};


#endif // MINIQBF_TYPES_H
