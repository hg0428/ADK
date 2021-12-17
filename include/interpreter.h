#pragma once
#include "common.h"
#include "ast.h"
#include "values.h"

namespace Aardvark {
  using std::string;
  using std::vector;
  using std::map;
  
  class Interpreter {
    public:
    AdkContext* ctx = nullptr;
    AST* ast;

    std::string curFile = "";

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

    AdkValue* iPlusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
    AdkValue* iMinusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
		AdkValue* iMultEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);
		AdkValue* iDivideEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj);

    AdkValue* iFunction(AST* expr, AdkObject* thisObj);
    AdkValue* iFunctionCall(AST* expr, AdkObject* thisObj);
    AdkValue* iClass(AST* expr);
    AdkValue* iString(AST* expr);
    AdkValue* iInt(AST* expr);
    AdkValue* iDouble(AST* expr);
    AdkValue* iScope(AST* expr);
    AdkValue* iAssign(AST* expr, AdkObject* thisObj);
    AdkValue* iIdentifier(AST* expr, AdkObject* thisObj);
    AdkValue* iInclude(AST* expr, AdkObject* thisObj);
  };

}; // namespace Aardvark