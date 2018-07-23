//
// Created by vedad on 07/10/17.
//

#ifndef NANOQBF_READER_H
#define NANOQBF_READER_H

#include "parseutils.h"
#include "types/Formula.h"

/// Reader class for parsing QDIMACS files into Formula objects
class Reader
{
private:
  StreamBuffer stream_;        ///< Input stream, from which the formula is read.
  QuantType type_;             ///< Quantifier type of the current quantifier
  std::vector<Var> variables_; ///< Variable quantified under the current quantifier
  std::vector<Lit> clause_;    ///< Current clause
  
  /// Reads and discards comments from #stream_
  void readComments();
  
  /// Reads QDIMACS header
  int readHeader(unsigned& nvars, unsigned& nclauses);
  
  /// Reads the whole prefix and adds it to \a f
  int readPrefix(Formula& f);
  
  /// Reads a single quantifier, called by readPrefix(Formula&)
  int readQuant(Formula& f);
  
  /// Reads the whole matrix and adds it to \a f
  int readMatrix(Formula& f);
  
  /// Reads a single clause, called by readMatrix(Formula&)
  int readClause();
  
  /// Parses an unsigned integer, which is a Var
  int parseUnsigned(unsigned& ret);
  
  /// Parses a signed integer, which is a Lit
  int parseSigned(int& ret);
public:
  /// Reads the whle QBF and stores it in \a f
  int readQBF(Formula& f);
  
  /// Reader Constructor
  Reader (gzFile& file) : stream_(file), type_(QuantType::NONE) {}
};


#endif // NANOQBF_READER_H
