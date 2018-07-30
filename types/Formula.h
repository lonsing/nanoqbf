//
// Created by vedad on 27/06/18.
//

#ifndef NANOQBF_FORMULA_H
#define NANOQBF_FORMULA_H

#include "common.h"
#include "Quant.h"
#include "Clause.h"

#include <iosfwd>
#include <vector>
#include <assert.h>

class Clause;

/// Class representing a quantified boolean formula
class Formula
{
public:
  /// Formula Constructor
  Formula();
  
  /// Formula Destructor
  ~Formula();
  
  /// Checks whether a given variable is quantified
  inline bool isQuantified(Var v) const;
  
  /// Checks whether a given variables is existentially quantified
  inline bool isExistential(Var v) const;
  
  /// Adds a Clause to #matrix
  /** Simplifies the given literal vector \a c, creates a Clause object in memory and adds it to #matrix.
   * @param c A vector of literals
   */
  void addClause(std::vector<Lit>& c);
  
  /// Adds an existential variable \a v to #free_variables
  void addFreeVar(Var v);
  
  /// Adds a Quant to #prefix
  int addQuantifier(QuantType type, std::vector<Var>& variables);
  
  /// Turns #free_variables into an existential Quant and calculates #position_offset
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
  inline int getVarDepth(Var v) const;
  
  /// Prints Formula \a f to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Formula& f);
  
  

private:
  
  std::vector<Quant*> prefix;  ///< Prefix of the QBF
  std::vector<Clause*> matrix; ///< Matrix of the QBF
  unsigned int num_exists;     ///< Number of existential variables
  unsigned int num_forall;     ///< Number of universal variables
  
  std::vector<int> quant_depth;            ///< Lookup table: \n index is the variable \n value is the quantifier
  std::vector<unsigned> quant_position;    ///< Lookup table: \n index is the variable \n value is position
  std::vector<unsigned> position_counters; ///< Lookup table: \n index is the qunatifier \n value is size
  std::vector<unsigned> position_offset;   ///< Lookup table: \n index is the quantifier \n value is phase offset
  
  std::vector<Var> free_variables; ///< Temporary vector of unquantified variables
  std::vector<Lit> tmp_exists;     ///< Temporary vector of existential variables
  std::vector<Lit> tmp_forall;     ///< Temporary vector of universal variables
  
  /// Quantifies variable \a v at quantifier with index \a depth
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

inline int Formula::getVarDepth(Var v) const
{
  assert(isQuantified(v));
  return quant_depth[v];
}

#endif //NANOQBF_FORMULA_H
