#include "aardvark.h"
#include <chrono>
#include <iostream>
#include <cstdint>

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using namespace Aardvark;

// I created a BigInt type that accepts a int64
AdkValue* adktime(std::vector<AdkValue*> args, Interpreter* i) {
	auto result = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	return new AdkValue((std::int64_t)result);
}
/*
As well as functions to time other functions and stuff like that.
*/
AdkValue* initTimeModule(Interpreter* i) {
  AdkObject* obj = (AdkObject*)i->gc->addValue(new AdkObject("time"));
	obj->Set("time", i->gc->addValue(new AdkFunction(i, adktime, "time")));

  return obj;
}