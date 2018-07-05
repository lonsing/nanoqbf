//
// Created by vedad on 27/06/18.
//

#ifndef NANOQBF_FORMULA_H
#define NANOQBF_FORMULA_H

#include "common.h"
#include "Quant.h"

#include <iosfwd>
#include <vector>
#include <assert.h>

class Clause;

class Formula
{
public:
  Formula();
  ~Formula();
  
  inline bool isQuantified(Var v) const;
  inline bool isExistential(Var v) const;
  
  void addClause(std::vector<Lit>& c);
  void addFreeVar(Var v);
  int addQuantifier(QuantType type, std::vector<Var>& variables);
  void finalise();
  
  inline unsigned long numVars() const;
  inline unsigned long numClauses() const;
  inline unsigned long numQuants() const;
  inline const Clause* getClause(unsigned index) const;
  inline const Quant* getQuant(unsigned index) const;
  inline unsigned int numExistential() const;
  inline unsigned int numUniversal() const;
  inline unsigned int getGlobalPosition(Var v) const;
  inline unsigned int getLocalPosition(Var v) const;
  inline int getDepth(Var v) const;
  
  friend std::ostream& operator<<(std::ostream& out, const Formula& f);

private:
  
  std::vector<Quant*> prefix;
  std::vector<Clause*> matrix;
  unsigned int num_exists;
  unsigned int num_forall;
  
  std::vector<int> quant_depth;
  std::vector<unsigned> quant_position;
  std::vector<unsigned> position_counters;
  std::vector<unsigned> position_offset;
  
  std::vector<Var> free_variables;
  std::vector<Lit> tmp_exists;
  std::vector<Lit> tmp_forall;
  
  int quantify(const Var v, unsigned depth);
};

////// INLINE FUNCTION DEFINITIONS //////

bool Formula::isQuantified(Var v) const
{
  return (v < quant_depth.size()) && (quant_depth[v] >= 0);
}

inline bool Formula::isExistential(Var v) const
{
  assert(isQuantified(v));
  return prefix[quant_depth[v]] == nullptr ||
         prefix[quant_depth[v]]->type == QuantType::EXISTS;
}

inline unsigned long Formula::numVars() const
{
  return quant_depth.size() - 1;
}

inline unsigned long Formula::numClauses() const
{
  return matrix.size();
}

inline unsigned long Formula::numQuants() const
{
  return prefix.size();
}

inline const Clause* Formula::getClause(unsigned index) const
{
  return matrix[index];
}

inline const Quant* Formula::getQuant(unsigned index) const
{
  return prefix[index];
}

inline unsigned int Formula::numExistential() const
{
  return num_exists;
}

inline unsigned int Formula::numUniversal() const
{
  return num_forall;
}

inline unsigned int Formula::getGlobalPosition(Var v) const
{
  assert(isQuantified(v));
  return quant_position[v] + position_offset[quant_depth[v]];
}

inline unsigned int Formula::getLocalPosition(Var v) const
{
  assert(isQuantified(v));
  return quant_position[v];
}

inline int Formula::getDepth(Var v) const
{
  assert(isQuantified(v));
  return quant_depth[v];
}

#endif //NANOQBF_FORMULA_H
