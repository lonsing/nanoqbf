# nanoqbf
A minimal implementation of an expansion-based QBF solver 
which does not use recursion.

To build the project you will need:
- CMake 3.10
- zlib
- a C++17 compliant compiler

Building NanoQBF in Release mode:
```
mkdir release && cd release;
cmake -DCMAKE_BUILD_TYPE=Release .. && make;
```

The build process will automatically download the Glucose 
SAT solver and build it. You should only get warnings while 
building Glucose, and not the main NanoQBF source files.
Feel free to file an issue if a a newer compiler breaks the
build process, as Glucose is already making problems and could
be replaced with a cleaner solver in the future.
