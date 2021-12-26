#include "aardvark.h"
#include <iostream>
#include <stdlib.h> 
#include <random> 
using namespace Aardvark;
using namespace std;
// I created a BigInt type that accepts a int64
AdkValue* adkRandInt(std::vector<AdkValue*> args, Interpreter* i) {
	int min = 0;
	int max = 10;
	if (args.size() == 1) {
			if (args[0]->type != DataTypes::Int) {
	        std::cout << "Error: \"randomInt\" only supports Interger arguments." << std::endl;
	        throw "err";
	  	}
			max = args[0]->toInt();
	}
	if (args.size() >= 2) {
			if (args[0]->type != DataTypes::Int || args[1]->type != DataTypes::Int ) {
	        std::cout << "Error: \"randomInt\" only supports Interger arguments." << std::endl;
	        throw "err";
	  	}
			min = args[0]->toInt();
			max = args[1]->toInt();
	}
	random_device rd;   // non-deterministic generator
  mt19937 gen(rd());  // to seed mersenne twister.
  uniform_int_distribution<> dist(min, max);
	int result = dist(gen);
	return new AdkValue(result);
}/*
AdkValue* adkRandInt(std::vector<AdkValue*> args, Interpreter* i) {
	int max = args[0].length;
	random_device rd;   // non-deterministic generator
  mt19937 gen(rd());  // to seed mersenne twister.
  uniform_int_distribution<> dist(min, max);
	int result = dist(gen);
	return new AdkValue(result);
}

As well as functions to time other functions and stuff like that.
*/
AdkValue* initRandomModule(Interpreter* i) {
  AdkObject* obj = (AdkObject*)i->gc->addValue(new AdkObject("random"));
	obj->Set("randomInt", i->gc->addValue(new AdkFunction(i, adkRandInt, "randomInt")));

  return obj;
}