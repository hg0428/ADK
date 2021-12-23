#include <cmath>
#include "gc.h"

// My best attempt at creating a garbage collector
// hg0428 – My attempt was worse. 
// TBH probably should have created this right in the beginning
// So much hard to do now

namespace Aardvark {

  AdkGC::AdkGC() {
    memory.reserve(maxArraySize);
  }

  AdkGC::AdkGC(int maxMem): maxMemory(maxMem) {
    maxArraySize = floor((maxMemory * (1024 * 1024)) / sizeof(AdkValue));
    memory.reserve(maxArraySize);
  }

  void AdkGC::setMaxMem(int maxMem) {
    maxMemory = maxMem;
    maxArraySize = floor((maxMemory * (1024 * 1024)) / sizeof(AdkValue));
    memory.reserve(maxArraySize);
  }

  AdkValue* AdkGC::addValue(AdkValue* item) {
    int memSize = memory.size() + 1;

    if (memSize >= maxArraySize) {
      std::cout << "Error: Not enough memory! Ran out of memory!" << std::endl;
      throw "err"; // Todo: Fix 'error'
    }

    // memory.push_back(item);

    return item;
  }

  void AdkGC::mark(AdkValue* value) {
    if (value->type == DataTypes::Object) {
      for (auto const& [key, val] : ((AdkObject*)value)->props) {
        mark(val.value);
      }
    }

    if (value->type == DataTypes::Class) {
      mark(((AdkClass*)value)->staticValues);
      mark(((AdkClass*)value)->thisObj);
    }

    value->isAccessible = true;
  }

  void AdkGC::sweep() {
    for (int i = 0; i < memory.size(); i++) {
      AdkValue* value = memory.at(i);
      if (value->isAccessible) {
        value->isAccessible = false;
        continue;
      }

      memory.erase(memory.begin() + i);

      // if (value != nullptr) {
      //   delete value;
      // }
      i--;
    }
  }

  void AdkGC::clean() {
    for (int i = 0; i < memory.size(); i++) {
      AdkValue* value = memory.at(i);

      if (value != nullptr) {
        delete value;
      }

      memory.erase(memory.begin() + i);
      i--;
    }
    memory.shrink_to_fit();
  }

  // The context after ctx = ctx->parent
  void AdkGC::collectGarbage(AdkContext* ctx, bool isSweeping = true) {
    if (ctx->parent != nullptr) {
      collectGarbage(ctx->parent, false);
    }
    
    for (auto const& [key, val] : ctx->variables) {
      mark(val.value);
    }

    if (isSweeping) {
      // sweep(); // Currently disabled. Has some bugs (double free errors)
    }
  }

}; // namespace Aardvark