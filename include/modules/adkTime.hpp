#include "aardvark.h"
#include <chrono>
#include <iostream>
#include <cstdint>
using namespace std::chrono;
using namespace Aardvark;

// I created a BigInt type that accepts a int64
AdkValue* adkTime(std::vector<AdkValue*> args, Interpreter* i) {
	std::string unit = "ERROR";
	auto result = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	if (args.size() < 1) {
     return new AdkValue((std::int64_t)result);
  } else {
			std::string unit = args[0]->toString();
			if (unit ==  "us") {
				result = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
			} else if (unit ==  "ms") {
				result = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "s") {
				result = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "min") {
				result = duration_cast<minutes>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "h") {
				result = duration_cast<hours>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "d") {
				result = duration_cast<days>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "w") {
				result = duration_cast<weeks>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "m") {
				result = duration_cast<months>(system_clock::now().time_since_epoch()).count();
			}
			else if (unit ==  "y") {
				result = duration_cast<years>(system_clock::now().time_since_epoch()).count();
			}
		return new AdkValue((std::int64_t)result);
	}
}
/*
As well as functions to time other functions and stuff like that.
*/
AdkValue* initTimeModule(Interpreter* i) {
  AdkObject* obj = (AdkObject*)i->gc->addValue(new AdkObject("time"));
	obj->Set("time", i->gc->addValue(new AdkFunction(i, adkTime, "time")));
  return obj;
}