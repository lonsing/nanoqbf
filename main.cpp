#include <iostream>
#include <zlib.h>

#include "types.h"
#include "Reader.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("Please specify an input file");
    return -1;
  }
  
  const char* filename = argv[argc - 1];
  gzFile file = gzopen(filename, "rb");
  
  if (file == Z_NULL)
  {
    printf("Could not open file: %s", filename);
    return -2;
  }
  
  Reader reader(file);
  Formula formula;
  
  if(reader.readQBF(formula)) return -3;
  gzclose(file);
  
  // std::cout << formula << std::endl;
  
  LOG("You will only see me in debug mode!");
  
  return 0;
}