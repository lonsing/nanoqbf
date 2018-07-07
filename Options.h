//
// Created by vedad on 06/07/18.
//

#ifndef NANOQBF_OPTIONS_H
#define NANOQBF_OPTIONS_H

#include <iosfwd>
#include <string>

struct Options
{
  Options();
  static const Options* make_options(int argc, const char* argv[]);
  friend std::ostream& operator<<(std::ostream& out, const Options& c);
  
  enum PruningMode {NONE, PERIODIC, DYNAMIC};
  
  unsigned int time_limit;
  
  PruningMode pruning;
  unsigned int pruning_period;
  unsigned int pruning_space;
  
  unsigned int warmup_samples;
  std::string file_name;
};


#endif //NANOQBF_OPTIONS_H
