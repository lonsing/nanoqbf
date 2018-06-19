//
// Created by vedad on 07/10/17.
//

#ifndef MINIQBF_TYPES_H
#define MINIQBF_TYPES_H

#include <vector>
#include <iostream>
#include <bitset>

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
  
  static Quant* make_quant(QuantType qtype, std::vector<Var>& vars);
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
  void set(int index, bool value);
  bool get(int index);
  
  void update(); // must be called after modifying the object
  friend bool operator==(const Assignment& lhs, const Assignment& rhs);
};

class Formula
{
public:
  ~Formula();
  
  inline bool isQuantified(Var v)
  {
    return (v < is_quantified.size()) && is_quantified[v];
  }
  
  inline bool isExistential(Var v)
  {
    return (v < is_existential.size()) && is_existential[v];
  }
  
  void addClause(std::vector<Lit>& c);
  void addFreeVar(Var v);
  int addQuantifier(QuantType type, std::vector<Var>& variables);
  
  inline unsigned long numVars()
  {
    return is_quantified.size() - 1;
  }
  
  inline unsigned long numClauses()
  {
    return matrix.size();
  }
  
  friend std::ostream& operator<<(std::ostream& out, const Formula& f);
  
private:
  
  std::vector<Quant*> prefix;
  std::vector<Clause*> matrix;
  std::vector<bool> is_quantified;
  std::vector<bool> is_existential;
  std::vector<Var> free_variables;
  std::vector<Lit> tmp_exists;
  std::vector<Lit> tmp_forall;
  
  int quantify(const Var v, const QuantType type);
};


#endif // MINIQBF_TYPES_H
