//
// Created by vedad on 07/10/17.
//

#include "Reader.h"

#include <assert.h>
#include <algorithm>

int Reader::readQBF(Formula& f)
{
  // read the header
  readComments();
  unsigned num_vars = 0;
  unsigned num_clauses = 0;
  
  if (readHeader(num_vars, num_clauses)) return -1;
  if (readPrefix(f)) return -2;
  if (readMatrix(f)) return -3;
  f.finalise();
  if (f.numVars() != num_vars)
    printf("c Warning: Variable count mismatch\n");
  if (f.numClauses() != num_clauses)
    printf("c Warning: Clause count mismatch\n");
  return 0;
}

void Reader::readComments()
{
  while (*stream_ == 'c') skipLine(stream_);
}

int Reader::readHeader(unsigned& nvars, unsigned& nclauses)
{
  if (!eagerMatch(stream_, "p cnf "))
  {
    printf("Error while reading header\n");
    return 3;
  }
  
  skipWhitespace(stream_);
  if (parseUnsigned(nvars)) return 3;
  if (parseUnsigned(nclauses)) return 3;
  return 0;
}

int Reader::readPrefix(Formula& f)
{
  while (true)
  {
    readComments();
    if (*stream_ != 'e' && *stream_ != 'a') break;
    if (readQuant(f)) return 4;
  }
  
  f.addQuantifier(type_, variables_);
  variables_.clear();
  
  return 0;
}

int Reader::readQuant(Formula& f)
{
  assert(*stream_ == 'e' || *stream_ == 'a');
  
  std::vector<bool> appearing_vars;
  
  QuantType type = (*stream_ == 'e') ? QuantType::EXISTS : QuantType::FORALL;
  
  ++stream_;
  skipWhitespace(stream_);
  
  if(type_ != type)
  {
    f.addQuantifier(type_, variables_);
    variables_.clear();
  }
  
  type_ = type;
  
  while (true)
  {
    if (*stream_ == EOF)
    {
      printf("Error when reading non-terminated quantifier\n");
      return 5;
    }
    Var v = 0;
    
    if (parseUnsigned(v)) return 5;
    
    if (v == 0) break;
    
    if (appearing_vars.size() <= v)
      appearing_vars.resize(v + 1, false);
    
    if (!appearing_vars[v])
      variables_.push_back(v);
    appearing_vars[v] = true;
  }
  
  return 0;
}

int Reader::readMatrix(Formula& f)
{
  
  while (true)
  {
    if (*stream_ == EOF) break;
    if (readClause()) return 6;
    f.addClause(clause_);
  }
  
  return 0;
}

int Reader::readClause()
{
  Lit l = 0;
  clause_.clear();
  while (true)
  {
    if (parseSigned(l)) return 7;
    if (l == 0) break;
    clause_.push_back(l);
  }
  return 0;
}

int Reader::parseUnsigned(unsigned& ret)
{
  if (*stream_ < '0' || *stream_ > '9')
  {
    printf("Error while reading unsigned number\n");
    return 1;
  }
  
  unsigned result = 0;
  while (*stream_ >= '0' && *stream_ <= '9')
  {
    assert(result <= result * 10 + (*stream_ - '0'));
    result = result * 10 + (*stream_ - '0');
    ++stream_;
  }
  ret = result;
  skipWhitespace(stream_);
  return 0;
}

int Reader::parseSigned(int& ret)
{
  int sign = 1;
  
  if (*stream_ == '-')
  {
    sign = -1;
    ++stream_;
  }
  
  if (*stream_ < '0' || *stream_ > '9')
  {
    printf("Error while reading signed number\n");
    return 2;
  }
  
  int result = 0;
  
  while (*stream_ >= '0' && *stream_ <= '9')
  {
    assert(result <= result * 10 + (*stream_ - '0'));
    result = result * 10 + (*stream_ - '0');
    ++stream_;
  }
  ret = sign * result;
  skipWhitespace(stream_);
  return 0;
}