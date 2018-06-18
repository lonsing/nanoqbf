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

#endif
