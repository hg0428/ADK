#pragma once
#ifndef RESWORD_H
#define RESWORD_H
#include "token.h"
//Add bitwise operations later on

namespace Aardvark {
  using std::vector;
  using std::string;
  // Operators
  // used in lexer.cpp and lexer.h
  // Strings on the right sets the alias
  // This allows mutli definitions of a single operator

  // Each array index is the length of the operators
  // Index 1 means each operator in that array are of length 2
  inline vector<std::map<string, string>> operators = {
    {
      {"=", "="},
      {"+", "+"},
      {"-", "-"},
      {"*", "*"},
      {"/", "/"},
      {"%", "%"},
      {"<", "<"},
      {">", ">"},
			{"^", "^"},
			{"!", "!"} //not
    },
    {
      {"+=", "+="},
      {"*=", "*="},
      {"/=", "/="},
      {"-=", "-="},
      {"==", "=="},
      {"!=", "!="},
      {"<=", "<="},
      {">=", ">="},
      {"++", "++"},
      {"--", "--"},
      {"x|", "x|"},
      {"||", "||"},
      {"&&", "&&"},
			{"or", "||"},
    },
    {
      {"xor", "x|"},
      {"and", "&&"},
    }
  };

  template<typename T>
	bool vincludes(std::vector<T> arr, T value) {
		return std::find(arr.begin(), arr.end(), value) != arr.end();
	}

  // These are operators with no right hand value or rvalue
  inline vector<string> noRightHand = {
    "++",
    "--"
  };
  inline vector<string> assignments = {
    "=",
    "++",
    "--",
    "+=",
    "-=",
		"*=",
		"/=",
		"%=", //modulus
		"**=", //exponet
		"|=" // assigns if left is none, else does nothing
  };

  // Order of operations. Precedence is defined by the lower value
  // Lower values are stronger than higher values 
  // Precedence is used in parser.cpp and parser.h
  inline std::map<string, int> precedence = {
    {"=",  1},
    {"+=", 2},
    {"++", 2},
    {"-=", 2},
    {"--", 2},
    {"*=", 2},
    {"/=", 2},
    {"|=", 2},
    {"&&", 4},
		{"and", 4},
    {"||", 5},
    {"or", 5},
		{"x|", 5},
		{"xor", 5},
    {"<",  7},
    {">",  7},
    {">=", 7},
    {"<=", 7},
    {"==", 7},
    {"!=", 7},
    {"+",  10},
    {"-",  10},
    {"*",  10}, // also all function are defined in lexer.h and token.h
    {"/",  10}, // it checks in lexer.cpp near the top and does the tokenizing in tokenize
    {"%",  10}, // Try it and the idea might come in my head and ill be able to help maybe
		{"//",  11}//Where is the code that checks for the symbols?
  };

  // Keywords
  // used in lexer.cpp and lexer.h
  inline std::map<string, TokenTypes> reservedWords = {
    {"funct", TokenTypes::Keyword},
    {"return", TokenTypes::Keyword},
    {"class", TokenTypes::Keyword},
    {"for", TokenTypes::Keyword},
    {"while", TokenTypes::Keyword},
    {"if", TokenTypes::Keyword},
    {"else", TokenTypes::Keyword},
		{"match", TokenTypes::Keyword}, //works like switch
		{"case", TokenTypes::Keyword},
		{"default", TokenTypes::Keyword},
    {"true", TokenTypes::Boolean},
    {"false", TokenTypes::Boolean},
		{"none", TokenTypes::None}, //We should have either none, null, or both
    {"as", TokenTypes::Keyword}, // Can only be used with a # statement, so it does not need to be reserved
    {"static", TokenTypes::Keyword},
    {"break", TokenTypes::Keyword},
    {"continue", TokenTypes::Keyword},
		{"static", TokenTypes::Keyword},
		{"reference", TokenTypes::Keyword},
		{"private", TokenTypes::Keyword}
  };
}; // namespace Aardvark

#endif