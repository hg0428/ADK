#include "adkcHeader/aardvark.h"
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