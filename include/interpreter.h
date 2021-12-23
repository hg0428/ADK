#pragma once
#include "common.h"
#include "ast.h"
#include "values.h"
#include "gc.h"

namespace Aardvark {
  using std::string;
  using std::vector;
  using std::map;
  
  class Interpreter {
    public:
    AdkGC* gc = nullptr;
    
    AdkContext* globalCtx = nullptr;
    AdkContext* ctx = nullptr;
    AST* ast;

    string curFile = "";
    string exePath = "";

    map<string, AdkValue*> modules;

    Interpreter();
    Interpreter(AST* ast);
    Interpreter(Interpreter& o);

    bool hasModule(string id);
    AdkValue* getModule(string id);
    void defineModule(string id, AdkValue* obj);

    AdkValue* GetDynamicLib(string fileName);

    void DefineGlobals(AdkContext* newCtx);

    AdkContext* EvalBuiltIn(string file, bool putInScope);
    AdkContext* EvalImport(string file, bool putInScope);
    AdkValue* Evaluate(bool builtIns);
    AdkValue* EvaluateFile(string file);

    AdkValue* interpret(AST* ast, AdkObject* thisObj);

    AdkValue* ConstructString(string data);

    // Helper Methods to Clean Code
    AdkValue* iPlusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
    AdkValue* iMinusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
		AdkValue* iMultEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
		AdkValue* iDivideEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);

    // Functions and Objects
    AdkValue* iFunction(AST* expr, AdkObject* thisObj);
    AdkValue* iFunctionCall(AST* expr, AdkObject* thisObj);
    AdkValue* iClass(AST* expr);
    AdkValue* iBinary(AST* expr);

    // Literals
    AdkValue* iString(AST* expr);
    AdkValue* iInt(AST* expr);
    AdkValue* iDouble(AST* expr);
    AdkValue* iBoolean(AST* expr);

    // Other
    AdkValue* iScope(AST* expr);
    AdkValue* iAssign(AST* expr, AdkObject* thisObj);
    AdkValue* iIdentifier(AST* expr, AdkObject* thisObj);
    AdkValue* iInclude(AST* expr, AdkObject* thisObj);
    AdkValue* iWhile(AST* expr, AdkObject* thisObj);
    AdkValue* iIf(AST* expr, AdkObject* thisObj);

    // Single keywords
    AdkValue* iNone(AST* expr);
    AdkValue* iReturn(AST* expr);
    AdkValue* iBreak(AST* expr);
    AdkValue* iContinue(AST* expr);
  };

}; // namespace Aardvark