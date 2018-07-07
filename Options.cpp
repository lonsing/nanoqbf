//
// Created by vedad on 06/07/18.
//

#include "Options.h"
#include "cxxopts.h"

#include <iostream>

Options::Options() :
  time_limit((unsigned)-1),
  memory_limit((unsigned)-1),
  pruning_a(PruningMode::DYNAMIC),
  pruning_b(PruningMode::PERIODIC),
  pparams_a(PruningParams((unsigned)-1)),
  pparams_b(PruningParams(50)),
  warmup_samples(16),
  file_name("")
{}

const Options* Options::make_options(int argc, const char* argv[])
{
  Options* opt = new Options();
  try
  {
    cxxopts::Options parse_opt(argv[0], " - An expansion-based QBF solver without recursion");
    parse_opt.positional_help("[optional args] filename").show_positional_help();
  
    parse_opt.add_options()
    ("t,time", "Time limit in seconds", cxxopts::value<int>())
    ("m,memory", "Memory limit in MB", cxxopts::value<int>())
    ("w,warmup", "Number of warmup assignments", cxxopts::value<int>())
    ("p,pruning-a", "Pruning mode A: none, periodic or dynamic", cxxopts::value<std::string>())
    ("q,pruning-b", "Pruning mode B: none, periodic or dynamic", cxxopts::value<std::string>())
    ("period-a", "Number of iterations inbetween prunings of A", cxxopts::value<int>())
    ("period-b", "Number of iterations inbetween prunings of B", cxxopts::value<int>())
    ("h,help", "Show this help menu")
    ("input", "Input file", cxxopts::value<std::string>(), "FILE");
  
    parse_opt.parse_positional({"input"});
    auto result = parse_opt.parse(argc, argv);
    
    if(result.count("help"))
    {
      delete opt;
      std::cout << parse_opt.help() << std::endl;
      return nullptr;
    }
    
    if (result.count("time"))
    {
      if(result["time"].as<int>() > 0)
        opt->time_limit = result["time"].as<unsigned>();
      else
        throw cxxopts::OptionException("Time limit must be positive");
    }
    
    if(result.count("memory"))
    {
      if(result["memory"].as<int>() > 0)
        opt->memory_limit = (unsigned)result["memory"].as<int>();
      else
        throw cxxopts::OptionException("Memory limit must be positive");
    }
    
    if(result.count("warmup"))
    {
      if(result["warmup"].as<int>() > 0)
        opt->warmup_samples = (unsigned)result["warmup"].as<unsigned>();
      else
        throw cxxopts::OptionException("Number of warmup samples must be positive");
    }
  
    if(result.count("pruning-a"))
      opt->pruning_a = parsePruning(result["pruning-a"].as<std::string>());
  
    if(result.count("pruning-b"))
      opt->pruning_b = parsePruning(result["pruning-b"].as<std::string>());
    
    if(result.count("period-a"))
    {
      if(result["period-a"].as<int>() > 0)
        opt->pparams_a.period = (unsigned)result["period-a"].as<unsigned>();
      else
        throw cxxopts::OptionException("Pruning period cannot be negative, use '-p none' instead");
    }
    
    if(result.count("period-b"))
    {
      if(result["period-b"].as<int>() > 0)
        opt->pparams_b.period = (unsigned)result["period-b"].as<unsigned>();
      else
        throw cxxopts::OptionException("Pruning period cannot be negative, use '-q none' instead");
    }
    
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

Options::PruningMode Options::parsePruning(std::string str_value)
{
  if(str_value == "none")
    return Options::PruningMode::NONE;
  else if(str_value == "periodic")
    return Options::PruningMode::PERIODIC;
  else if(str_value == "dynamic")
    return Options::PruningMode::DYNAMIC;
  else
    throw cxxopts::OptionException("Cannot recognise value " + str_value);
}


std::ostream& operator<<(std::ostream& out, const Options& opt)
{
  out << "c Options:" << std::endl
      << "c   time      = " << opt.time_limit << std::endl
      << "c   memory    = " << opt.memory_limit << std::endl
      << "c   warmup    = " << opt.warmup_samples << std::endl
      << "c   pruning-a = " << opt.pruning_a << std::endl
      << "c    period-a = " << opt.pparams_a.period << std::endl
      << "c   pruning-b = " << opt.pruning_b << std::endl
      << "c    period-b = " << opt.pparams_b.period << std::endl
      << "c   input    = " << opt.file_name << std::endl;
  return out;
}
