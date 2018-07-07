//
// Created by vedad on 06/07/18.
//

#ifndef NANOQBF_OPTIONS_H
#define NANOQBF_OPTIONS_H

#include <iosfwd>
#include <string>

struct PruningParams
{
  unsigned int period;
  inline PruningParams(unsigned p) : period(p) {};
};



struct Options
{
  enum PruningMode {NONE, PERIODIC, DYNAMIC};
  
  Options();
  static const Options* make_options(int argc, const char* argv[]);
  static PruningMode parsePruning(std::string str_value);
  friend std::ostream& operator<<(std::ostream& out, const Options& c);
  
  unsigned int time_limit;
  unsigned int memory_limit;
  PruningMode pruning_a;
  PruningMode pruning_b;
  PruningParams pparams_a;
  PruningParams pparams_b;
  unsigned int warmup_samples;
  std::string file_name;
};


#endif //NANOQBF_OPTIONS_H
