//
// Created by vedad on 17/06/18.
//

#ifndef MINIQBF_LOGGER_H
#define MINIQBF_LOGGER_H

#ifndef NDEBUG

struct Logger
{
  static void log (const char * fmt, ...);
};

#define LOG(FMT, ARGS...) \
do { \
  Logger::log (FMT, ##ARGS); \
} while (0)

#else
#define LOG(FMT, ARGS...) do { } while (0)
#endif

#endif //MINIQBF_LOGGER_H
