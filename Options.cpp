//
// Created by vedad on 06/07/18.
//

#include "Options.h"
#include "cxxopts.h"

#include <iostream>

Options::Options() :
  time_limit((unsigned)-1),
  pruning(PruningMode::PERIODIC),
  pruning_period(100),
  pruning_space((unsigned)-1),
  warmup_samples(16)
{}

const Options* Options::make_options(int argc, const char* argv[])
{
  Options* opt = new Options();
  try
  {
    cxxopts::Options parse_opt(argv[0], " - An expansion-based QBF solver without recursion");
    parse_opt.positional_help("[optional args] filename").show_positional_help();
  
    parse_opt.add_options()
    ("t,time", "Time limit in seconds", cxxopts::value<unsigned>())
    ("p,pruning", "Pruning mode: none, periodic or dynamic", cxxopts::value<std::string>())
    ("pruning-space", "Space limit in MB", cxxopts::value<unsigned>())
    ("pruning-period", "Number of iterations inbetween prunings", cxxopts::value<unsigned>())
    ("w,warmup", "Number of warmup assignments", cxxopts::value<unsigned>())
    ("input", "Input file", cxxopts::value<std::string>(), "FILE");
    
    parse_opt.parse_positional({"input"});
    auto result = parse_opt.parse(argc, argv);
    
    if(result.count("time"))
      opt->time_limit = result["time"].as<unsigned>();
    if(result.count("pruning"))
    {
      std::string pruning = result["pruning"].as<std::string>();
      if(pruning == "none")
        opt->pruning = Options::PruningMode::NONE;
      else if(pruning == "periodic")
        opt->pruning = Options::PruningMode::PERIODIC;
      else if(pruning == "dynamic")
        opt->pruning = Options::PruningMode::DYNAMIC;
      else
        throw cxxopts::OptionException("Cannot recognise value" + pruning);
    }
    if(result.count("pruning-space"))
      opt->pruning_space = result["pruning-space"].as<unsigned>();
    if(result.count("pruning-period"))
      opt->pruning_period = result["pruning-period"].as<unsigned>();
    if(result.count("warmup"))
      opt->warmup_samples = result["warmup"].as<unsigned>();
    if(result.count("input"))
      opt->file_name = result["input"].as<std::string>();
    else
    {
      delete opt;
      std::cout << parse_opt.help() << std::endl;
      return nullptr;
    }
    
    std::cout << *opt << std::endl;
    return opt;
  }
  catch (const cxxopts::OptionException& e)
  {
    delete opt;
    std::cout << "c Error while reading program options" << std::endl;
    return nullptr;
  }
}

std::ostream& operator<<(std::ostream& out, const Options& opt)
{
  out << "c Options:" << std::endl
      << "c   time     = " << opt.time_limit << std::endl
      << "c   space    = " << opt.pruning_space << std::endl
      << "c   warmup   = " << opt.warmup_samples << std::endl
      << "c   pruning  = " << opt.pruning << std::endl
      << "c    -space  = " << opt.pruning_space << std::endl
      << "c    -period = " << opt.pruning_period << std::endl
      << "c   input    = " << opt.file_name << std::endl;
  return out;
}
