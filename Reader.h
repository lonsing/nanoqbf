//
// Created by vedad on 07/10/17.
//

#ifndef NANOQBF_READER_H
#define NANOQBF_READER_H

#include "parseutils.h"
#include "types/Formula.h"

class Reader
{
private:
  StreamBuffer stream_;
  QuantType type_;
  std::vector<Var> variables_;
  std::vector<Lit> clause_;
  
  void readComments();
  int readHeader(unsigned& nvars, unsigned& nclauses);
  int readPrefix(Formula& f);
  int readQuant(Formula& f);
  int readMatrix(Formula& f);
  int readClause();
  int parseUnsigned(unsigned& ret);
  int parseSigned(int& ret);
public:
  int readQBF(Formula& f);
  Reader (gzFile& file) : stream_(file), type_(QuantType::NONE) {}
};


#endif // NANOQBF_READER_H
