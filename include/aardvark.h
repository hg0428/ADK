#pragma once
#ifndef ADK_H
#define ADK_H
#include "interpreter.h"

namespace Aardvark {
  extern "C" {
    struct Definition {
      const char* name;
      NativeFunction function;
      int argCount;
      AdkValue* value = nullptr;
    };
    struct ModuleInfo {
      const char* moduleName;
      std::vector<Definition> definitions;
    };
    class AdkModule {
      public:
      ModuleInfo* info;
      AdkValue* obj;

      AdkModule(ModuleInfo* info, AdkValue* obj): info(info), obj(obj) {};
    };
  };

  typedef AdkModule* (*InitFunction)(Interpreter* interp);
}; // namespace Aardvark

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define ADK_EXPORT extern "C" __declspec(dllexport)
#define ADK_INIT extern "C" __declspec(dllexport) Aardvark::AdkModule*
#else
#define ADK_EXPORT
#define ADK_INIT extern "C" Aardvark::AdkModule*
#endif

Aardvark::AdkModule* CreateModule(Aardvark::ModuleInfo* info);

#endif