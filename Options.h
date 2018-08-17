//
// Created by vedad on 06/07/18.
//

#ifndef NANOQBF_OPTIONS_H
#define NANOQBF_OPTIONS_H

#include <iosfwd>
#include <string>

/// Parameters for NanoQBF::pruneCheckA() and NanoQBF::pruneCheckB()
struct PruningParams
{
  unsigned int period;
  inline PruningParams(unsigned p) : period(p) {};
};

/// Runtime options passed to NanoQBF
struct Options
{
  /// Specifies pruning strategy
  enum PruningMode {NONE, PERIODIC, DYNAMIC};
  
  /// Options Constructor
  Options();
  
  /// Constructs an Options object in memory from command line parameters
  static const Options* make_options(int argc, const char* argv[]);
  
  /// Parses \a str_value into a PruningMode value
  static PruningMode parsePruning(std::string str_value);
  
  /// Prints Options \a opt to output stream \a out
  friend std::ostream& operator<<(std::ostream& out, const Options& opt);
  
  unsigned int time_limit;     ///< Non-binding time limit for NanoQBF::solve()
  unsigned int memory_limit;   ///< Non-binding memory limit for NanoQBF
  PruningMode pruning_a;       ///< Pruning strategy used in NanoQBF::pruneCheckA()
  PruningMode pruning_b;       ///< Pruning strategy used in NanoQBF::pruneCheckB()
  PruningParams pparams_a;     ///< Pruning parameters used in NanoQBF::pruneCheckA()
  PruningParams pparams_b;     ///< Pruning parameters used in NanoQBF::pruneCheckB()
  unsigned int warmup_samples; ///< Number of initial sub-formulas generated in NanoQBF::initA()
  bool structured_warmup;      ///< Whether structured warmup will be done in NanoQBF::initA()
  bool covering_warmup;        ///< Whether covering warmup will be done in NanoQBF::initA()
  std::string file_name;       ///< File name of the QDIMACS file containing the QBF
};

#endif //NANOQBF_OPTIONS_H
