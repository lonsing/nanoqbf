//
// Created by vedad on 17/06/18.
//

#ifndef NANOQBF_LOGGER_H
#define NANOQBF_LOGGER_H

#include <vector>
#include "types/Assignment.h"

#ifndef NDEBUG

/// Logging utility for DEBUG mode builds
struct Logger
{
  /// Logs a printf style format string to stdout
  static void log (const char * fmt, ...);
  
  /// Logs a clause and then a printf style format string to stdout
  static void log_cl (std::vector<Lit>& clause, const char * fmt, ...);
};

#define LOG(FMT, ARGS...) \
do { \
  Logger::log (FMT, ##ARGS); \
} while (0)

#define LOG_CL(CL, FMT, ARGS...) \
do { \
  Logger::log_cl (CL, FMT, ##ARGS); \
} while (0)

#else
#define LOG(FMT, ARGS...) do { } while (0)
#define LOG_CL(CL, FMT, ARGS...) do { } while (0)
#endif

#endif //NANOQBF_LOGGER_H
