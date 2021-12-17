#include "common.h"
#include "parser.h"
#include "aardvark.h"
#include "fileHandler.hpp"
#include "loadlib.hpp"

namespace fs = std::filesystem;

namespace Aardvark {

  bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
  }

  bool isNumber(std::string token)
  {
      return std::regex_match(token, std::regex(("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
  }

  // AdkValue
  AdkValue::AdkValue() {
    this->type = DataTypes::None;
    this->data = nullptr;
  }
  AdkValue::AdkValue(DataTypes type) {
    this->type = type;
    this->data = nullptr;
  }
  AdkValue::AdkValue(string val) {
    this->type = DataTypes::String;
    this->data = new std::string(val);
  }
  AdkValue::AdkValue(int val) {
    this->type = DataTypes::Int;
    this->data = new int(val);
  }
  AdkValue::AdkValue(double val) {
    this->type = DataTypes::Double;
    this->data = new double(val);
  }

  AdkValue* AdkValue::operator+(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() + val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();
        return new AdkValue(((int)valData) + val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() + val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        double thisVal = cast<double>();
        int secondVal = val.cast<int>(); // it was like this. That makes sense

        return new AdkValue(thisVal + ((double)secondVal));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator-(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() - val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();
        return new AdkValue(((double)valData) - val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() - val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() - ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator*(AdkValue& val) {
		if (type == DataTypes::String) {
			//Note for the future: "test"*3 = "testtesttest" to be added
		}
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() * val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();
        return new AdkValue(((double)valData) * val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() * val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() * ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator/(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() / val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) / val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() / val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() / ((double)valData));
      }
    }

    return this;
  }

  string AdkValue::toString() {
    switch(type) {
      case DataTypes::String:
        return cast<string>();
        
      case DataTypes::Object: {
        AdkObject* cls = (AdkObject*)this;
        if (cls->objType == "String") {
          AdkValue* length = cls->Get("length").value;
          string str = "";

          if (length->type == DataTypes::None) length = new AdkValue(0);

          for (int i = 0; i < length->cast<int>(); i++) {
            str += cls->Get(std::to_string(i)).value->toString();
          }

          return str;
        }

        // well you could probably do something like added other operators and maybe working in the parser and lexer
        // the parser is pretty much a bunch of if statements and class structures
        // its recursive so its just a bunch of funtion calls
        // try implementing if statements
				//Ok, I will try that, if I get the chance today.
        if (cls->objType.size() > 0) {
          std::ostringstream str;
          str << "[Object ";
          str << cls->objType;
          str << "]";

          return str.str();
        }
        return "[Object unknown]";
      }
      case DataTypes::Int:
        return std::to_string(cast<int>());

      case DataTypes::Double: {
        std::ostringstream str;
        str << cast<double>();
        return str.str();
      }

      case DataTypes::None:
        return "[None]";

      default:
        std::ostringstream str;
        str << "&[" << this->data;
        str << "]";
        return str.str();
    }

    return "[Unknown]";
  }

  // AdkObject
  AdkObject::AdkObject(): AdkValue(DataTypes::Object) {
    this->objType = "None";
  }

  AdkObject::AdkObject(string o): AdkValue(DataTypes::Object) {
    this->objType = o;
  }

  bool AdkObject::isNone() {
    string str = "None";
    
    return (this->objType == str);
  }

  // Variable
  Variable::Variable() {
    this->name = "None";
    this->value = new AdkValue();
  }

  Variable::Variable(string name) {
    this->name = name;
    this->value = new AdkValue();
  }
  Variable::Variable(string name, AdkValue* value) {
    this->name = name;
    this->value = value;
  }

  // AdkContext
  AdkContext::AdkContext() {
    this->parent = nullptr;
  };

  AdkContext::AdkContext(AdkContext* parent) {
    this->parent = parent;
  }

  // AdkFunction
  AdkFunction::AdkFunction(Interpreter* interp, AST* exp): AdkValue(DataTypes::Function) {
    this->info = exp;
    this->name = exp->value.toString();
    this->interpreter = new Interpreter(*interp);
  }
  AdkFunction::AdkFunction(Interpreter* interp, NativeFunction func, string name): AdkValue(DataTypes::Function) {
    this->func = func;
    this->name = name;
    this->interpreter = new Interpreter(*interp);
  }

  AdkValue* AdkFunction::Call(vector<AST*> args, AdkValue* thisObj) {
    vector<AdkValue*> evalArgs = {};

    for (AST* arg : args) {
      AdkValue* evaledArg = interpreter->interpret(arg, new AdkObject());
      evalArgs.push_back(evaledArg);
    }

    return Call(evalArgs, thisObj);
  }

  AdkValue* AdkFunction::Call(vector<AdkValue*> args, AdkValue* thisObj) {
    AdkContext oldCtx = *interpreter->ctx;
    interpreter->ctx = new AdkContext();
    interpreter->ctx->parent = oldCtx.parent;
    interpreter->ctx->variables = oldCtx.variables;

    interpreter->ctx->Set("this", thisObj);

    if (func != nullptr) {
      AdkValue* returnValue = func(args, interpreter);

      interpreter->ctx = oldCtx.Extend();

      return returnValue;
    }

    for (int i = 0; i < info->args.size(); i++) {
      AST* argDef = info->args[i];

      if (i >= args.size()) {
        interpreter->ctx->Set(argDef->value.toString(), new AdkValue());
        continue;
      }

      interpreter->ctx->Set(argDef->value.toString(), args[i]);
    }

    AdkValue* returnValue = new AdkValue();

    for (AST* exp : info->block) {
      returnValue = interpreter->interpret(exp, new AdkObject());
    }

    interpreter->ctx = oldCtx.Extend();

    return returnValue;
  }

  // AdkClass
  AdkClass::AdkClass() {
    this->className = "_anon_";
    this->thisObj = new AdkObject();
    this->interpreter = nullptr;
  }
  AdkClass::AdkClass(Interpreter* interp, string className): AdkValue(DataTypes::Class) {
    this->className = className;
    this->thisObj = new AdkObject();
    this->interpreter = interp;
  }
  AdkClass::AdkClass(Interpreter* interp, string className, AdkObject* thisObj): AdkValue(DataTypes::Class) {
    this->className = className;
    this->thisObj = thisObj;
    this->interpreter = interp;
  }

  AdkValue* AdkClass::NewInstance(vector<AdkValue*> args) {
    AdkObject* obj = thisObj->Copy();

    AdkFunction* func = (AdkFunction*)thisObj->Get("~init").value;
	
    func->interpreter = interpreter;
    
    func->Call(args, obj);
    return obj;
  }

  // Interpreter
  AdkValue* Interpreter::iScope(AST* expr) {
    for (AST* exp : expr->block) {
      AdkValue* returnVal = interpret(exp, new AdkObject());

      if (returnVal != nullptr && returnVal->returnedVal) {
        return returnVal;
      }
    }

    return new AdkValue();
  }

  AdkValue* Interpreter::iClass(AST* expr) {
    string name = expr->value.toString();
    AdkClass* cls = new AdkClass(this, name, new AdkObject(name));

    AdkContext* oldCtx = ctx;
    ctx = ctx->Extend();
    for (AST* exp : expr->block) {
      AdkValue* value = interpret(exp, cls->thisObj);
      cls->thisObj->Set(exp->value.toString(), value);
    }
    // cls->props = ctx->variables;
    ctx = oldCtx;

    return ctx->Set(expr->value.toString(), (AdkValue*)cls).value;
  }

  AdkValue* Interpreter::iFunction(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->value;
    string name = identifier.toString();

    AdkFunction* func = new AdkFunction(this, expr);

    if (thisObj->isNone()) {
      ctx->Set(name, func);
    } else {
      thisObj->Set(name, func);
    }

    return (AdkValue*)func;
  }

  AdkValue* Interpreter::iPlusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj) {
    Token identifier = expr->left->value;
    string name = identifier.toString();
    bool useCtx = expr->parent == nullptr && thisObj->isNone();

    // the lValue object
    AdkValue* value = useCtx
      ? ctx->Get(name).value
      : thisObj->Get(name).value;

    AdkValue* oldValue = value;


    if (value->type == DataTypes::Object) {
      AdkObject* obj = (AdkObject*)value;

      if (obj->objType != "String") {
        throw "Fail";
      }

      int rValSize = rValue->toString().size();
      int lValSize = value->toString().size();
      string rVal = rValue->toString();

      for (int i = lValSize; i < rValSize + lValSize; i++) {
        string cStr = std::string(1, rVal[i - lValSize]);
        AdkValue* c = new AdkValue(cStr);

        ((AdkObject*)value)->Set(std::to_string(i), c);
      }

      ((AdkObject*)value)->Set("length", new AdkValue(rValSize + lValSize));
    }


    if (value->type == DataTypes::Int || value->type == DataTypes::Double) {
      value = *value + *rValue;
    }


    if (useCtx) {
      AdkValue* retVal = ctx->Set(name, value).value;

      // Checks if the operator is something like ++identifier or identifer++
      return expr->isOpBefore ? retVal : oldValue;
    } else {
      AdkValue* retVal = thisObj->Set(name, value).value;

      // Checks if the operator is something like ++identifier or identifer++
      return expr->isOpBefore ? retVal : oldValue;
    }
  }

  AdkValue* Interpreter::iMinusEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->left->value;
    string name = identifier.toString();
    bool useCtx = expr->parent == nullptr && thisObj->isNone();

    // the lValue object
    AdkValue* value = useCtx
      ? ctx->Get(name).value
      : thisObj->Get(name).value;

    AdkValue* oldValue = value;


    if (value->type == DataTypes::Int || value->type == DataTypes::Double) {
      value = *value - *rValue;
    }


    if (useCtx) {
      AdkValue* retVal = ctx->Set(name, value).value;

      // Checks if the operator is something like --identifier or identifer--
      return expr->isOpBefore ? retVal : oldValue;
    } else {
      AdkValue* retVal = thisObj->Set(name, value).value;

      // Checks if the operator is something like --identifier or identifer--
      return expr->isOpBefore ? retVal : oldValue;
    }
  }
	AdkValue* Interpreter::iMultEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->left->value;
    string name = identifier.toString();
    bool useCtx = expr->parent == nullptr && thisObj->isNone();

    // the lValue object
    AdkValue* value = useCtx
      ? ctx->Get(name).value
      : thisObj->Get(name).value;
		//Add string mode later: "test"*=3 becomes "testtesttest"
    if (value->type == DataTypes::Int || value->type == DataTypes::Double) {
      value = *value * *rValue;
    }


    if (useCtx) {
      return ctx->Set(name, value).value;
    } else {
      return thisObj->Set(name, value).value;
    }
  }
	AdkValue* Interpreter::iDivideEquals(AST* expr, AdkValue* rValue, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->left->value;
    string name = identifier.toString();
    bool useCtx = expr->parent == nullptr && thisObj->isNone();

    // the lValue object
    AdkValue* value = useCtx
      ? ctx->Get(name).value
      : thisObj->Get(name).value;
		//Add string mode later: "test"*=3 becomes "testtesttest"
    if (value->type == DataTypes::Int || value->type == DataTypes::Double) {
      value = *value / *rValue;
    }


    if (useCtx) {
      return ctx->Set(name, value).value;
    } else {
      return thisObj->Set(name, value).value;
    }
  }

  AdkValue* Interpreter::iAssign(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->left->value;
    string name = identifier.toString();
    // Assignments with no right value
    if (expr->value.toString() == "++") {
      return iPlusEquals(expr, new AdkValue(1), thisObj);
    } else if (expr->value.toString() == "--") {
      return iMinusEquals(expr, new AdkValue(1), thisObj);
    }

    bool useCtx = expr->parent == nullptr && thisObj->isNone();
    AdkValue* rValue = interpret(expr->right, new AdkObject());

    // if (rValue->type == DataTypes::Int)
    //   std::cout << "DEBUG: " << rValue->cast<double>() << std::endl;

    if (expr->value.toString() == "+=") {
      return iPlusEquals(expr, rValue, thisObj);
    } else if (expr->value.toString() == "-=") {
      return iMinusEquals(expr, rValue, thisObj);
    } else if (expr->value.toString() == "*=") {
      return iMultEquals(expr, rValue, thisObj);
    } else if (expr->value.toString() == "/=") {
      return iDivideEquals(expr, rValue, thisObj);
    }


    if (useCtx) {
      return ctx->Set(name, rValue).value;
    } else {
      return thisObj->Set(name, rValue).value;
    }
  }

  AdkValue* Interpreter::iIdentifier(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token mainIdentifier = expr->value;
    string name = mainIdentifier.toString();
    
    AdkValue* value = (expr->parent == nullptr || thisObj->isNone())
      ? ctx->Get(name).value
      : thisObj->Get(name).value;

    if (expr->access != nullptr) {
      // Checks if the value is accessible
      if (value->type != DataTypes::Object && value->type != DataTypes::String) {
        throw "Cannot read property of undefined."; // Todo: Fix 'error'
      }

      // Checks on types to access a value
      if (expr->access->type == ASTTypes::Int) {
        // Converts int into string and accesses
        AdkValue* newValue = interpret(expr->access, (AdkObject*)value);
        string test = std::to_string(newValue->cast<int>());

        return ((AdkObject*)value)->Get(test).value;
      } else if (expr->access->type == ASTTypes::String) {
        // Access property using a string
        string n = expr->access->value.toString();

        return ((AdkObject*)value)->Get(n).value;
      } else if (expr->access->isSubscript) {
        AdkValue* indexValue = interpret(expr->access, new AdkObject());

        return ((AdkObject*)value)->Get(indexValue->toString()).value;
      }

      AdkValue* prop = interpret(expr->access, (AdkObject*)value);

      return prop;
    }
    
    return value;
  }

  AdkValue* Interpreter::iFunctionCall(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->value;
    // std::cout << "Get: " << identifier.toString() << "<< END;" << std::endl;
    
    AdkValue* func = (expr->parent == nullptr || thisObj->isNone())
      ? ctx->Get(identifier.toString()).value
      : thisObj->Get(identifier.toString()).value;

    vector<AdkValue*> args = {};

    for (AST* arg : expr->args) {
      AdkValue* evaledArg = interpret(arg, new AdkObject());

      args.push_back(evaledArg);
    }


    if (func->type == DataTypes::Class) {
      ((AdkClass*)func)->interpreter = new Interpreter(*this);
      
      return ((AdkClass*)func)->NewInstance(args);
    } else {
      ((AdkFunction*)func)->interpreter = new Interpreter(*this);

      return (AdkValue*)((AdkFunction*)func)->Call(args, thisObj);
    }
  }

  AdkValue* Interpreter::ConstructString(string data) {
    AdkClass* strCls = (AdkClass*)ctx->Get("String").value;
    vector<AdkValue*> args = {};
    args.push_back(new AdkValue(data));

    strCls->interpreter = new Interpreter(*this);
    AdkObject* str = (AdkObject*)strCls->NewInstance(args);

    for (int i = 0; i < data.length(); i++) {
      string cStr = std::string(1, data[i]);
      AdkValue* c = new AdkValue(cStr);
      str->Set(std::to_string(i), c);
      // Loop through string and set every character at current index
    }

    str->Set("length", new AdkValue((int)data.length()));

    return (AdkValue*)str;
  }

  AdkValue* Interpreter::iString(AST* expr) {
    AdkClass* strCls = (AdkClass*)ctx->Get("String").value;
    string data = expr->value.toString();

    vector<AdkValue*> args = {};
    args.push_back(new AdkValue(data));

    strCls->interpreter = new Interpreter(*this);
    AdkObject* str = (AdkObject*)strCls->NewInstance(args);

    for (int i = 0; i < data.length(); i++) {
      string cStr = std::string(1, data[i]);
      AdkValue* c = new AdkValue(cStr);
      str->Set(std::to_string(i), c);
      // Loop through string and set every character at current index
    }

    str->Set("length", new AdkValue((int)data.length()));

    return (AdkValue*)str;
  }

  AdkValue* Interpreter::iInt(AST* expr) {
    Token numberTok = expr->value;
    int number = std::stoi(numberTok.toString());

    return new AdkValue(number);
  }

  AdkValue* Interpreter::iDouble(AST* expr) {
    Token numberTok = expr->value;
    double number = std::stod(numberTok.toString());

    return new AdkValue(number);
  }

  AdkValue* Interpreter::GetDynamicLib(string fileName) {
    DynamicLib lib = DynamicLib(fileName);

    lib.OpenLib();

    std::shared_ptr<InitFunction> init = lib.GetSymbol<InitFunction>("initmodule");
    AdkModule* module = (*init)(this);
    ModuleInfo* info = module->info;
    AdkObject* obj = new AdkObject("C ImpCModule");

    const char* moduleName = info->moduleName;
    std::vector<Definition> definitions = info->definitions;

    for (Definition def : definitions) {
      const char* defName = def.name;
      NativeFunction func = def.function;
      int argCount = def.argCount;
      AdkValue* rvalue = def.value;

      if (func == nullptr) {
        obj->Set(defName, rvalue);
      } else {
        obj->Set(defName, new AdkFunction(this, func, defName));
      }
    }

    return (AdkValue*)obj;
  }

  AdkValue* Interpreter::iInclude(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token fileNameTok = expr->value;
    string fileName = fileNameTok.toString();

    fs::path fullfile = fs::path(curFile).remove_filename() / fs::absolute(fs::path(fileName)).filename();
    std::string data = fullfile.string();

    bool isDynamicLib = false;

    if (hasEnding(fileName, std::string(".adc"))) {
      isDynamicLib = true;
    }

    if (hasModule(fileName)) {
      if (expr->as == nullptr) {
        std::cout << "Error: Builtin modules can only be assigned to a variable. No variable identified." << std::endl;
        throw "ERR";
      }

      if (expr->as->type != ASTTypes::Identifier) {
        std::cout << "Error: Cannot place context of module that is not a variable." << std::endl;
        throw "err";
      }

      AdkValue* obj = getModule(fileName);

      return ctx->Set(expr->as->value.toString(), obj).value;
    }

    if (expr->as != nullptr) {
      if (expr->as->type != ASTTypes::Identifier) {
        std::cout << "Error: Cannot place context of file that is not a variable." << std::endl;
        throw "err";
      }

      string identifier = expr->as->value.toString();

      if (isDynamicLib) {
        return ctx->Set(identifier, GetDynamicLib(data)).value;
      }

      AdkContext* fileCtx = EvalImport(data, false);
      AdkObject* ctxObj = new AdkObject("__Include__");

      for (auto const& [key, val] : fileCtx->variables) {
        ctxObj->Set(key, val.value);
      }

      return ctx->Set(identifier, ctxObj).value;
    }

    if (isDynamicLib) {
      AdkObject* obj = (AdkObject*)GetDynamicLib(data);

      for (auto const& [key, val] : obj->props) {
        ctx->Set(key, val.value);
      }

      return (AdkValue*)obj;
    }

    EvalImport(data, true);

    return new AdkValue();
  }

  AdkValue* Interpreter::interpret(AST* expr, AdkObject* thisObj = new AdkObject()) {
    switch (expr->type) {
      case ASTTypes::Scope:
        return iScope(expr);

      case ASTTypes::Class:
        return iClass(expr);

      case ASTTypes::Function:
        return iFunction(expr, thisObj);
      
      case ASTTypes::FunctionCall:
        return iFunctionCall(expr, thisObj);

      case ASTTypes::Assign:
        return iAssign(expr, thisObj);

      case ASTTypes::Identifier:
        return iIdentifier(expr, thisObj);

      case ASTTypes::String:
        return iString(expr);

      case ASTTypes::Int:
        return iInt(expr);

      case ASTTypes::Double:
        return iDouble(expr);

      case ASTTypes::Include:
        return iInclude(expr, thisObj);
    }
  }

  bool Interpreter::hasModule(string id) {
    return modules.find(id) != modules.end();
  }
  AdkValue* Interpreter::getModule(string id) {
    if (!hasModule(id)) {
      std::cout << "Error: Module '" << id << "' not found." << std::endl; 
      throw "Err";
    }

    return modules[id];
  }

  AdkContext* Interpreter::EvalImport(string file, bool putInScope = true) {
    string info = readFile(file);

    Lexer lexer = Lexer();
    std::vector<Token> tokens = lexer.tokenize(info);

    Parser parser = Parser(tokens);
    AST* tree = parser.parse();

    Interpreter* interp = new Interpreter(tree);
    interp->curFile = file;

    interp->Evaluate(true);

    if (putInScope) {
      for (auto const& [key, val] : interp->ctx->variables) {
        ctx->Set(key, val.value);
      }
    }

    return interp->ctx;
  }

  void Interpreter::defineModule(string id, AdkValue* obj) {
    modules[id] = obj;
  }

  // Evaluates a file
  // Takes the global ctx and sets all definitions into the current ctx
  AdkContext* Interpreter::EvalBuiltIn(string file, bool putInScope = true) {
    string info = readFile(file);

    Lexer lexer = Lexer();
    std::vector<Token> tokens = lexer.tokenize(info);

    Parser parser = Parser(tokens);
    AST* tree = parser.parse();

    Interpreter* interp = new Interpreter(tree);
    interp->curFile = file;

    interp->Evaluate(false);

    if (putInScope) {
      for (auto const& [key, val] : interp->ctx->variables) {
        ctx->Set(key, val.value);
      }
    }

    return interp->ctx;
  }

  AdkValue* Interpreter::EvaluateFile(string file) {
    string info = readFile(file);

    Lexer lexer = Lexer();
    std::vector<Token> tokens = lexer.tokenize(info);

    Parser parser = Parser(tokens);
    AST* tree = parser.parse();

    Interpreter* interp = new Interpreter(tree);
    interp->curFile = file;
    interp->ctx = ctx;

    return interp->Evaluate(true);
  }

  void Interpreter::DefineGlobals(AdkContext* newCtx) {
    newCtx->Set("output", new AdkFunction(this, [](vector<AdkValue*> args, Interpreter* interp) {
      for (AdkValue* arg : args) {
        std::cout << arg->toString() << " ";
      }

      std::cout << "\n";
      
      return new AdkValue();
    }, "output"));
  }

  AdkValue* Interpreter::Evaluate(bool builtIns = false) {
    if (builtIns) {
      EvalBuiltIn("./builtin/String.adk", true);

      DefineGlobals(ctx);
    }
    
    return interpret(this->ast);
  }

  Interpreter::Interpreter() {
    this->ast = nullptr;
    this->ctx = new AdkContext();
  }

  Interpreter::Interpreter(AST* ast) {
    this->ast = ast;
    this->ctx = new AdkContext();
  }

  Interpreter::Interpreter(Interpreter& o) {
    this->ast = o.ast;
    this->ctx = o.ctx;
  }

}; // namespace Aardvark