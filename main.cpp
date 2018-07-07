#include <iostream>
#include <zlib.h>

#include "NanoQBF.h"
#include "Reader.h"
#include "Logger.h"
#include "Options.h"

int main(int argc, const char* argv[])
{
  const Options* options = Options::make_options(argc, argv);
  if (options == nullptr)
    return -1;
  
  gzFile file = gzopen(options->file_name.c_str(), "rb");
  
  if (file == Z_NULL)
  {
    printf("Could not open file: %s", options->file_name.c_str());
    return -2;
  }
  
  Reader reader(file);
  Formula formula;
  
  if(reader.readQBF(formula)) return -3;
  gzclose(file);
  NanoQBF solver(&formula, options);
  
  return solver.solve();
}