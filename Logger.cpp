//
// Created by vedad on 17/06/18.
//

#ifndef NDEBUG

#include "Logger.h"
#include "stdarg.h"
#include "stdio.h"

void Logger::log(const char* fmt, ...)
{
  va_list ap;
  printf ("c LOG ");
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  fputc ('\n', stdout);
  fflush (stdout);
}

void Logger::log_cl(std::vector<Lit>& clause, const char* fmt, ...)
{
  va_list ap;
  printf ("c LOG ");
  
  for(unsigned i = 0; i < clause.size(); i++)
    printf("%d ", clause[i]);
  
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  fputc ('\n', stdout);
  fflush (stdout);
}


#endif
