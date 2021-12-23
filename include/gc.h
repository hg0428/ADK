#pragma once
#include "common.h"
#include "parser.h"
#include "values.h"

namespace Aardvark {
  using std::string;
  using std::vector;

  class AdkGC {
    public:

    AdkGC();
    AdkGC(int maxMem);

    void setMaxMem(int maxMem);

    AdkValue* addValue(AdkValue* item);
    void collectGarbage(AdkContext* ctx, bool isSweeping);
    void clean();

    private:
    int maxMemory = 10000; // in KB (1000KB = 1MB)
    int maxArraySize = (maxMemory * (1024)) / sizeof(AdkValue);
    vector<AdkValue*> memory;

    void mark(AdkValue* val);
    void sweep();
  };

}; // namespace Aardvark