# Aardvark

## Compiling

To compile aardvark requires C++20. No dependecies required besides dynamic linking libraries.

Compile with:
```bash
g++ -std=c++20 src/*.cpp -I./include -ldl -o aardvark
```

## Creating a C++ Library

Compiling a C++ Aardvark library takes a few steps


**First** include "adkcHeader/adk.h"
- This files contains the module library setup definitions

**Second** have the aardvark interpreter source files and headers

**Third** compile everything
```bash
g++ -std=c++20 -fPIC -c -ldl -I./include src/*.cpp library.cpp
```

```bash
g++ -shared *.o -o library.adc
```

**Example:**
```c++
#include "adkcHeader/adk.h"
#include <string>
#include <vector>

using namespace Aardvark;

AdkValue* printLol(std::vector<AdkValue*> args, Interpreter* i) {
  std::cout << "Lol" << std::endl;

  return new AdkValue();
}

std::vector<Definition> definitions = {
  { "printLol", printLol, 0 }
};

ModuleInfo modInfo = {
  "hithere",
  definitions
};

ADK_INIT initmodule(Interpreter* i) {
  modInfo.definitions = definitions;
  return CreateModule(&modInfo);
}
```

**NOTE:**
Any time there is an update you will have to recompile the module for that update.
This means you will have to get the newest source files.