#include "aardvark.h"
#include <chrono>
#include <iostream>
#include <cstdint>
using namespace std::chrono;
using namespace Aardvark;

// I created a BigInt type that accepts a int64
AdkValue* adkTime(std::vector<AdkValue*> args, Interpreter* i) {
	auto result = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	return new AdkValue((std::int64_t)result);
}
/*
As well as functions to time other functions and stuff like that.
*/
AdkValue* initTimeModule(Interpreter* i) {
  AdkObject* obj = (AdkObject*)i->gc->addValue(new AdkObject("time"));
	obj->Set("time", i->gc->addValue(new AdkFunction(i, adkTime, "time")));
	//obj->Set("local", i->gc->addValue(new AdkFunction(i, adkLocalTime, "local")));

  return obj;
}