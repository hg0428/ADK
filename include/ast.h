#pragma once
#include "lexer.h"

namespace Aardvark {
  using std::string;
  using std::vector;

  enum class ASTTypes {
    None, // 0
    While, // 1
    For, // 2
    If, // 3
    Int, // 4
    Double, // 5
    String, // 6
    Boolean, // 7
    Class, // 8
    Object, // 9
    Array, // 10
    Variable, // 11
    Import, // 12 What is this one for? 
    Identifier, // 13
    Assign, // 14
    Binary, // 15
    Scope, // 16
    FunctionCall, // 17
    Function, // 18
    AnonFunction, // 19
    FunctionDecl, // 20
    Return, // 21
    Continue, // 22
    Include, // 23
    Break // 24
  };

  class AST {
    public:
    ASTTypes type;
    Token value;

    // Binary
    AST* left;
    Token op;
    AST* right;
    bool isOpBefore = false;

    // Scopes and Functions
    vector<AST*> block;
    vector<AST*> args;
    AST* scope;

    bool isStatic = false;

    AST* as = nullptr;
  
    // If
    AST* condition;
    AST* then;
    AST* els;

    // For
    AST* reassign;
    AST* assign;

    // Identifier
    AST* access = nullptr;
    AST* parent = nullptr;
    bool isSubscript = false;

    AST();
    AST(Token val);
    AST(ASTTypes type, Token val);

    string toString();
    bool isNull();

    private:
  };

}; // namespace Aardvark