#include "common.h"
#include "ast.h"
#include "parser.h"
#include "aardvark.h"
#include "gc.h"
#include "fileHandler.hpp"
#include "loadlib.hpp"
#include <sstream>
#include <stdexcept>
namespace fs = std::filesystem;

namespace Aardvark {

  // Helper functions
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

  bool isDouble(std::string token) {
    return std::regex_match(token, std::regex(("(\\+|-)?[:digit:]+\\.[:digit:]+")));
  }

  // AdkValue -> Aka the Main top node Value class -> Everything "value" inherits it
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
  AdkValue::AdkValue(int64_t val) {
    this->type = DataTypes::BigInt;
    this->data = new int64_t(val);
  }
  AdkValue::AdkValue(int val) {
    this->type = DataTypes::Int;
    this->data = new int(val);
  }
  AdkValue::AdkValue(double val) {
    this->type = DataTypes::Double;
    this->data = new double(val);
  }
  AdkValue::AdkValue(bool val) {
    this->type = DataTypes::Boolean;
    this->data = new bool(val);
  }

  AdkValue* AdkValue::operator+(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() + val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();
        return new AdkValue(((int)valData) + val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() + val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();
        return new AdkValue(((int64_t)valData) + val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() + val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        double thisVal = cast<double>();
        int secondVal = val.cast<int>();

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
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() - val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();
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
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() * val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();
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
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() / val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

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

  AdkValue* AdkValue::operator%(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() % val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        return new AdkValue(cast<int>() % ((int)val.cast<double>()));
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() % val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        return new AdkValue(cast<int64_t>() % ((int64_t)val.cast<double>()));
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(((int)cast<double>()) % ((int)val.cast<double>()));
      } else if (val.type == DataTypes::Int) {
        return new AdkValue(((int)cast<double>()) % val.cast<int>());
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator>(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() > val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) > val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() > val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) > val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() > val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() > ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator>=(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() >= val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) >= val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() >= val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) >= val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() >= val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() >= ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator==(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() == val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) == val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() == val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) == val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() == val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() == ((double)valData));
      }
    } else if (type == DataTypes::Boolean) {
      if (val.type == DataTypes::Boolean) {
        return new AdkValue(toBool() == val.toBool());
      }
    }

    return new AdkValue(false);
  }

  AdkValue* AdkValue::operator!=(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() != val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) != val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() != val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) != val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() != val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() != ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator<(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() < val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) < val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() < val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) < val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() < val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() < ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator<=(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() <= val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) <= val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() <= val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) <= val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() <= val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() <= ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator&&(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() && val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) && val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() && val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) && val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() && val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() && ((double)valData));
      }
    }

    return this;
  }

  AdkValue* AdkValue::operator||(AdkValue& val) {
    if (type == DataTypes::Int) {
      if (val.type == DataTypes::Int) {
        return new AdkValue(cast<int>() || val.cast<int>());
      } else if (val.type == DataTypes::Double) {
        int valData = cast<int>();

        return new AdkValue(((double)valData) || val.cast<double>());
      }
    } else if (type == DataTypes::BigInt) {
      if (val.type == DataTypes::BigInt) {
        return new AdkValue(cast<int64_t>() || val.cast<int64_t>());
      } else if (val.type == DataTypes::Double) {
        int64_t valData = cast<int64_t>();

        return new AdkValue(((double)valData) || val.cast<double>());
      }
    } else if (type == DataTypes::Double) {
      if (val.type == DataTypes::Double) {
        return new AdkValue(cast<double>() || val.cast<double>());
      } else if (val.type == DataTypes::Int) {
        int valData = val.cast<int>();
        return new AdkValue(cast<double>() || ((double)valData));
      }
    }

    return this;
  }

  bool AdkValue::toBool() {
    // Checks the type of the object and converts it accordingly
    switch(type) {
      case DataTypes::Boolean:
        return cast<bool>();

      case DataTypes::Int:
        return (cast<int>() > 0);

      case DataTypes::Double:
        return (cast<double>() > 0.0);
			
			case DataTypes::String:
        return (cast<string>() != "");
      default:
        return false;
    }
  }

  int AdkValue::toInt() {
    // Checks the type of the object and converts it accordingly
    switch(type) {
      case DataTypes::Int:
        return cast<int>();

      case DataTypes::Double:
        return (int)(cast<double>());

      case DataTypes::Boolean:
        return (int)(cast<bool>());

      case DataTypes::BigInt:
        return (int)(cast<int64_t>());

      default:
        return 0;
    }
  }

  double AdkValue::toDouble() {
    // Checks the type of the object and converts it accordingly
    switch(type) {
      case DataTypes::Double:
        return cast<double>();

      case DataTypes::Int:
        return (double)(cast<int>());

      case DataTypes::Boolean:
        return (double)(cast<bool>());

      default:
        return 0.0;
    }
  }

  // This method 'toString()' will create a string representation
  // of the current object/value

  // Probably will change to an overridable method
  // So it will be cleaner code
  // Maybe even make AdkNumber class and AdkString classes
  // but that will be for later
  string AdkValue::toString() {
    switch(type) {
      case DataTypes::String:
        return cast<string>();

      case DataTypes::Class: {
        std::ostringstream str;
        str << "[Class " << (((AdkClass*)this)->className) << "]";

        return str.str();
      }
        
      case DataTypes::Object: {
        AdkObject* cls = (AdkObject*)this;
        if (cls->objType == "String") { // AdkObject String to raw string
          AdkValue* length = cls->Get("length").value;
          string str = "";

          if (length->type == DataTypes::None) length = new AdkValue(0);

          for (int i = 0; i < length->cast<int>(); i++) {
            str += cls->Get(std::to_string(i)).value->toString();
          }

          return str;
        }

        if (cls->objType.size() > 0) {
          std::ostringstream str;
					//Change to [ClassName VariableName]
					//Not all classes are objects, except in JS.
          str << "[Object ";
          str << cls->objType;
          str << "]";

          return str.str();
        }
        return "[Object unknown]";
      }
      case DataTypes::Int:
        return std::to_string(cast<int>());

      case DataTypes::BigInt: {
        std::ostringstream str;
        str << cast<int64_t>();
        return str.str();
      }

      case DataTypes::Double: {
        std::ostringstream str;
        str << cast<double>();
        return str.str();
      }

      case DataTypes::Boolean: {
        return cast<bool>() ? "true" : "false";
      }

      case DataTypes::None:
        return "null";

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
    this->interpreter->gc = interp->gc;
  }
  AdkFunction::AdkFunction(Interpreter* interp, NativeFunction func, string name): AdkValue(DataTypes::Function) {
    this->func = func;
    this->name = name;
    this->interpreter = new Interpreter(*interp);
    this->interpreter->gc = interp->gc;
    this->info = new AST(ASTTypes::None, Token(TokenTypes::Identifier, name));
  }

  AdkValue* AdkFunction::Call(vector<AST*> args, AdkValue* thisObj) {
    vector<AdkValue*> evalArgs = {}; // The interpreted arguments

    // Loops through each AST* argument (which is just an uninterpreted, parsed arg)
    for (AST* arg : args) {
      // Evaluates the argument and pushes it to the list
      AdkValue* evaledArg = interpreter->interpret(arg, new AdkObject());
      evalArgs.push_back(evaledArg);
    }

    // Does another call but this time with the evaluated arguments
    // See the overloaded method below 
    return Call(evalArgs, thisObj);
  }

  AdkValue* AdkFunction::Call(vector<AdkValue*> args, AdkValue* thisObj) {
    // AdkContext oldCtx = *interpreter->ctx;
    // interpreter->ctx = new AdkContext();
    // interpreter->ctx->parent = oldCtx.parent;
    // interpreter->ctx->variables = oldCtx.variables;
    AdkObject* currentThis = interpreter->ctx->HasVar("this")
      ? (AdkObject*)interpreter->ctx->Get("this").value
      : new AdkObject();

    string funcName = info->value.toString();
    bool isManipulator = funcName[0] == '~' || (
      currentThis->HasProp("name") &&
      currentThis->Get("name").value->toString() == "~on_funct_call"
    );

    // This is quite messy and maybe should be implemented different
    // This is the implementation of manipulating the global scope
    // Basically creating event listeners for specific things
    // This one is for funtion calls
    // if (
    //   !isManipulator &&
    //   interpreter->globalCtx->HasVar("~on_funct_call")
    //  ) {
    //   AdkFunction* on_funct_call = (AdkFunction*)interpreter->globalCtx->Get("~on_funct_call").value;

    //   AdkObject* functionInfo = new AdkObject(funcName);
    //   functionInfo->Set("name", interpreter->ConstructString(funcName));

    //   vector<AdkValue*> eventArgs = {
    //     (AdkValue*)functionInfo
    //   };

    //   AdkObject* newObj = new AdkObject("~on_funct_call");
    //   newObj->Set("name", interpreter->ConstructString("~on_funct_call"));

      
    //   interpreter->ctx = interpreter->ctx->Extend();
    //   on_funct_call->Call(eventArgs, newObj);
    //   interpreter->ctx = interpreter->ctx->parent;
    // }

    // Extends the current context aka the 'scope'
    interpreter->ctx = interpreter->ctx->Extend();

    interpreter->ctx->Set("this", thisObj); // Sets the 'this' value

    if (func != nullptr) {
      // Calls the Native C++ function
      AdkValue* returnValue = func(args, interpreter);

      // Returns to the old scope
      interpreter->ctx = interpreter->ctx->parent;
      // interpreter->ctx = oldCtx.Extend();

      return returnValue;
    }

    // Interprets all passed arguments / parameters
    // Only interprets up to the function max paramlist
    // eg: funct test(x, y) {}; -> only has 2 params so max is 2
    for (int i = 0; i < info->args.size(); i++) {
      AST* argDef = info->args[i];

      // if there is only a few arguments passed the rest will be 'none' aka 'null'
      /* eg:
          funct test(x, y) {};

          test(5) // -> the y argument will be 'none' \\
      */
      if (i >= args.size()) {
        // argDef is an AST* type and to get the name of that argument is
        // stored in the token called value
        interpreter->ctx->Set(argDef->value.toString(), interpreter->gc->addValue(new AdkValue()));
        continue;
      }

      // Sets the argument in the current scopes variable list with the interpreted argument
      interpreter->ctx->Set(argDef->value.toString(), args[i]);
    }

    AdkValue* returnValue = interpreter->gc->addValue(new AdkValue());

    for (AST* exp : info->block) {
      // Interprets every value in the function body
      returnValue = interpreter->interpret(exp, new AdkObject());

      if (returnValue->returned) {
        break;     
      }
    }

    // Returns to the old scope
    interpreter->ctx = interpreter->ctx->parent;
    if (interpreter->gc != nullptr) {
      interpreter->gc->collectGarbage(interpreter->ctx, true);
    }
    // interpreter->ctx = oldCtx.Extend();

    return returnValue;
  }

  // AdkClass
  AdkClass::AdkClass() {
    this->className = "_anon_";
    this->thisObj = new AdkObject();
    this->staticValues = new AdkObject("_anon_");
    this->interpreter = nullptr;
  }
  AdkClass::AdkClass(Interpreter* interp, string className): AdkValue(DataTypes::Class) {
    this->className = className;
    this->thisObj = new AdkObject();
    this->staticValues = new AdkObject(className);
    this->interpreter = interp;
  }
  AdkClass::AdkClass(Interpreter* interp, string className, AdkObject* thisObj): AdkValue(DataTypes::Class) {
    this->className = className;
    this->thisObj = thisObj;
    this->staticValues = new AdkObject(className);
    this->interpreter = interp;
  }

  // Creates a new Instance of a class
  AdkValue* AdkClass::NewInstance(vector<AdkValue*> args) {
    AdkObject* obj = thisObj->Copy();
    // Copies the 'obj instructions' or the defined obj with default values
    // so that every instance has its own values

    // Gets the initial 'constructor' method
    AdkFunction* func = (AdkFunction*)thisObj->Get("~init").value;
	
    // sets the interpreter so it can be accessed later
    func->interpreter = interpreter;
    
    // calls the 'constructor' / 'initial' method
    func->Call(args, obj);
    return obj; // returns the new object
  }

  // Interpreter
  AdkValue* Interpreter::iScope(AST* expr) {
    // Loops through each expression in the current scope
    // and evaluates it
    for (AST* exp : expr->block) {
      AdkValue* returnVal = interpret(exp, new AdkObject());

      if (returnVal != nullptr && returnVal->returned) {
        return returnVal;
      }
    }

    return gc->addValue(new AdkValue());
  }

  AdkValue* Interpreter::iClass(AST* expr) {
    string name = expr->value.toString();
    AdkClass* cls = (AdkClass*)gc->addValue(new AdkClass(this, name, new AdkObject(name)));

    AdkContext* oldCtx = ctx;
    ctx = ctx->Extend();
    for (AST* exp : expr->block) {
      AdkValue* value = interpret(exp, cls->thisObj);

      // Checks if it is a function and is a static class member
      // if it is it assigns it as a static member instead of a regular
      // property
      if (value->type == DataTypes::Function) {
        bool isStatic = ((AdkFunction*)value)->isStatic;
        
        if (isStatic) {
          // value->isAccessible = true;
          cls->Set(exp->value.toString(), value);
          continue;
        }
      }

      // Sets the value property to the obj
      cls->thisObj->Set(exp->value.toString(), value); 
    }
    // cls->props = ctx->variables;
    ctx = ctx->parent;
    ctx->Set(expr->value.toString(), (AdkValue*)cls).value;

    // Defines the 'class' in the current scope
    return (AdkValue*)cls;
  }

  AdkValue* Interpreter::iFunction(AST* expr, AdkObject* thisObj = new AdkObject()) {
    Token identifier = expr->value;
    string name = identifier.toString();

    AdkFunction* func = (AdkFunction*)gc->addValue(new AdkFunction(this, expr));
    func->isStatic = expr->isStatic;

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
        AdkValue* c = gc->addValue(new AdkValue(cStr));

        ((AdkObject*)value)->Set(std::to_string(i), c);
      }

      ((AdkObject*)value)->Set("length", gc->addValue(new AdkValue(rValSize + lValSize)));
    }


    if (value->type == DataTypes::Int || value->type == DataTypes::Double) {
      value = *value + *rValue;
      gc->addValue(value);
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
      return iPlusEquals(expr, gc->addValue(new AdkValue(1)), thisObj);
    } else if (expr->value.toString() == "--") {
      return iMinusEquals(expr, gc->addValue(new AdkValue(1)), thisObj);
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
      if (
        value->type != DataTypes::Object &&
        value->type != DataTypes::String &&
        value->type != DataTypes::Class
      ) {
        std::cout << "Error: Cannot read property of primitive values" << std::endl;
        throw "Cannot read property of undefined."; // Todo: Fix 'error'
      }

      if (value->type == DataTypes::Class) {
        value = ((AdkClass*)value)->staticValues;
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
    
    AdkValue* func = (expr->parent == nullptr && thisObj->isNone())
      ? ctx->Get(identifier.toString()).value
      : thisObj->Get(identifier.toString()).value;

    if (func->isNone()) {
      std::cout << "Error: Cannot call value that is undefined!" << std::endl; // Todo: Fix "error"
      throw "err";
    }

    vector<AdkValue*> args = {};

    for (AST* arg : expr->args) {
      AdkValue* evaledArg = interpret(arg, new AdkObject());
      evaledArg->isAccessible = true;

      args.push_back(evaledArg);
    }


    if (func->type == DataTypes::Class) {
      ((AdkClass*)func)->interpreter = new Interpreter(*this);
      ((AdkClass*)func)->interpreter->gc = gc;
      func->isAccessible = true;
      
      return ((AdkClass*)func)->NewInstance(args);
    } else {
      ((AdkFunction*)func)->interpreter = new Interpreter(*this);
      ((AdkFunction*)func)->interpreter->gc = gc;

      return (AdkValue*)((AdkFunction*)func)->Call(args, thisObj);
    }
  }

  AdkValue* Interpreter::ConstructString(string data) {
    AdkClass* strCls = (AdkClass*)ctx->Get("String").value;
    vector<AdkValue*> args = {};
    args.push_back(gc->addValue(new AdkValue(data)));

    strCls->interpreter = new Interpreter(*this);
    strCls->interpreter->gc = gc;
    AdkObject* str = (AdkObject*)strCls->NewInstance(args);

    for (int i = 0; i < data.length(); i++) {
      string cStr = std::string(1, data[i]);
      AdkValue* c = gc->addValue(new AdkValue(cStr));
      str->Set(std::to_string(i), c);
      // Loop through string and set every character at current index
    }

    str->Set("length", gc->addValue(new AdkValue((int)data.length())));

    return (AdkValue*)str;
  }

  AdkValue* Interpreter::iString(AST* expr) {
    AdkClass* strCls = (AdkClass*)ctx->Get("String").value;
    string data = expr->value.toString();

    vector<AdkValue*> args = {};
    args.push_back(gc->addValue(new AdkValue(data)));

    strCls->interpreter = new Interpreter(*this);
    strCls->interpreter->gc = gc;
    AdkObject* str = (AdkObject*)strCls->NewInstance(args);

    for (int i = 0; i < data.length(); i++) {
      string cStr = std::string(1, data[i]);
      AdkValue* c = gc->addValue(new AdkValue(cStr));
      str->Set(std::to_string(i), c);
      // Loop through string and set every character at current index
    }

    str->Set("length", gc->addValue(new AdkValue((int)data.length())));

    return (AdkValue*)str;
  }

  AdkValue* Interpreter::iInt(AST* expr) {
    Token numberTok = expr->value;
    int number = std::stoi(numberTok.toString());

    return gc->addValue(new AdkValue(number));
  }

  AdkValue* Interpreter::iDouble(AST* expr) {
    Token numberTok = expr->value;
    double number = std::stod(numberTok.toString());

    return gc->addValue(new AdkValue(number));
  }

  AdkValue* Interpreter::iBoolean(AST* expr) {
    Token boolTok = expr->value;
    bool boolean = boolTok.toString() == "true";

    return gc->addValue(new AdkValue(boolean));
  }

  AdkValue* Interpreter::GetDynamicLib(string fileName) {
    DynamicLib lib = DynamicLib(fileName);

    lib.OpenLib();

    std::shared_ptr<InitFunction> init = lib.GetSymbol<InitFunction>("initmodule");
    AdkModule* module = (*init)(this);
    ModuleInfo* info = module->info;
    AdkObject* obj = new AdkObject("C AdkCModule");

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
      AdkValue* obj = getModule(fileName);

      if (obj->type != DataTypes::Object) {
        std::cout << "Built in C++ modules must return an AdkObject!" << std::endl;
        throw "err";
      }

      if (expr->as == nullptr) {
        for (auto const& [ key, val ] : ((AdkObject*)obj)->props) {
          ctx->Set(key, val.value).value;
        }

        return obj;
      }

      if (expr->as->type != ASTTypes::Identifier) {
        std::cout << "Error: Cannot place context of module that is not a variable." << std::endl;
        throw "err";
      }


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

    return gc->addValue(new AdkValue());
  }

  AdkValue* Interpreter::iWhile(AST* expr, AdkObject* thisObj = new AdkObject()) {
    // AST* condition = expr->condition;
    // Bug every loop variables get reset
    // This happens because they are copied this needs to be fixed
    // at least for the closures
    while (true) {
      if (!interpret(expr->condition, thisObj)->toBool()) {
        break;
      }

      ctx = ctx->Extend();
      for (AST* exp : expr->block) {

        if (exp->type == ASTTypes::Break) {
          ctx = ctx->parent;
          return gc->addValue(new AdkValue());
        } else if (exp->type == ASTTypes::Continue) {
          break; // this breaks out the for loop basically doing a continue
        }

        AdkValue* returnVal = interpret(exp, thisObj);

        if (returnVal->returned) {
          ctx = ctx->parent;
          return returnVal;
        }
      }
      ctx = ctx->parent;
    }

    // if (gc != nullptr) {
    //   gc->collectGarbage(ctx, true);
    // }

    return gc->addValue(new AdkValue());
  }

  AdkValue* Interpreter::iNone(AST* expr) {
    return gc->addValue(new AdkValue());
  }

  AdkValue* Interpreter::iReturn(AST* expr) {
    AdkValue* returnVal = interpret(expr->assign, new AdkObject());
    returnVal->returned = true;
    returnVal->isAccessible = true;

    return returnVal;
  }

  AdkValue* Interpreter::iBreak(AST* expr) {
    AdkValue* breakVal = gc->addValue(new AdkValue());
    breakVal->breaked = true;

    return breakVal;
  }

  AdkValue* Interpreter::iContinue(AST* expr) {
    AdkValue* continueVal = gc->addValue(new AdkValue());
    continueVal->continued = true;

    return continueVal;
  }

  AdkValue* Interpreter::iIf(AST* expr, AdkObject* thisObj) {
    AST* conditionAST = expr->condition;
    AdkValue* condition = interpret(conditionAST, thisObj);

    if (condition->toBool()) {
      for (AST* exp : expr->block) {
        AdkValue* returnValue = interpret(exp, thisObj);

        if (
          returnValue->returned ||
          returnValue->continued ||
          returnValue->breaked
        ) {
          return returnValue;
        }
      }
    } else if (expr->els != nullptr) {
      for (AST* exp : expr->els->block) {
        AdkValue* returnValue = interpret(exp, thisObj);

        if (
          returnValue->returned ||
          returnValue->continued ||
          returnValue->breaked
        ) {
          return returnValue;
        }
      }
    }

    return gc->addValue(new AdkValue());
  }

  AdkValue* Interpreter::iBinary(AST* expr) {
    AdkValue* leftValue = interpret(expr->left, new AdkObject());
    string op = expr->op.toString();

    // These should interpret the right if only the left is evaulated
    if (op == "||") {
      if (leftValue->toBool()) {
        return gc->addValue(new AdkValue(true));
      }
      
      AdkValue* rightValue = interpret(expr->right, new AdkObject());

      return gc->addValue(*leftValue || *rightValue);
    } else if (op == "&&") {
      if (!leftValue->toBool()) {
        return gc->addValue(new AdkValue(false));
      }

      AdkValue* rightValue = interpret(expr->right, new AdkObject());

      return gc->addValue(*leftValue && *rightValue);
    }

    AdkValue* rightValue = interpret(expr->right, new AdkObject());

    // yea idk if this is the best way to do it but oh well
    // maybe make a map with function ptrs and do it that way but this works for now

    // Todo: Make this better
    if (op == "+") {
      return gc->addValue(*leftValue + *rightValue);
    } else if (op == "-") {
      return gc->addValue(*leftValue - *rightValue);
    } else if (op == "*") {
      return gc->addValue(*leftValue * *rightValue);
    } else if (op == "/") {
      return gc->addValue(*leftValue / *rightValue);
    } else if (op == "%") {
      return gc->addValue(*leftValue % *rightValue);
    } else if (op == "-") {
      return gc->addValue(*leftValue - *rightValue);
    } else if (op == "==") {
      return gc->addValue(*leftValue == *rightValue);
    } else if (op == "!=") {
      return gc->addValue(*leftValue != *rightValue);
    } else if (op == ">") {
      return gc->addValue(*leftValue > *rightValue);
    } else if (op == "<") {
      return gc->addValue(*leftValue < *rightValue);
    } else if (op == ">=") {
      return gc->addValue(*leftValue >= *rightValue);
    } else if (op == "<=") {
      return gc->addValue(*leftValue <= *rightValue);
    }

    std::cout << "Error: unrecognized operator '" << op << "'" << std::endl;
    throw "err"; // Todo: Fix 'error'
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

      case ASTTypes::Binary:
        return iBinary(expr);

      case ASTTypes::String:
        return iString(expr);

      case ASTTypes::Boolean:
        return iBoolean(expr);

      case ASTTypes::Int:
        return iInt(expr);

      case ASTTypes::Double:
        return iDouble(expr);

      case ASTTypes::Include:
        return iInclude(expr, thisObj);

      case ASTTypes::While:
        return iWhile(expr, thisObj);

      case ASTTypes::None:
        return iNone(expr);

      case ASTTypes::Return:
        return iReturn(expr);

      case ASTTypes::Break:
        return iBreak(expr);

      case ASTTypes::Continue:
        return iContinue(expr);

      case ASTTypes::If:
        return iIf(expr, thisObj);
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
    interp->modules = modules;
    interp->exePath = exePath;
    interp->gc = new AdkGC();

    interp->Evaluate(true);

    if (putInScope) {
      for (auto const& [key, val] : interp->ctx->variables) {
        ctx->Set(key, val.value);
      }
    }

    AdkContext* importCtx = interp->ctx;

    interp->gc->collectGarbage(ctx, true);
    delete interp->gc;
    delete interp;

    return importCtx;
  }

  void Interpreter::defineModule(string id, AdkValue* obj) {
    modules[id] = obj;
  }

  // Evaluates a file
  // Takes the global ctx and sets all definitions into the current ctx
  AdkContext* Interpreter::EvalBuiltIn(string file, bool putInScope = true) {
    string pathToFile = (fs::path(exePath).remove_filename() / fs::path("../") / file).string();
    string info = readFile(pathToFile);

    Lexer lexer = Lexer();
    std::vector<Token> tokens = lexer.tokenize(info);

    Parser parser = Parser(tokens);
    AST* tree = parser.parse();

    Interpreter* interp = new Interpreter(tree);
    interp->curFile = file;
    interp->modules = modules;
    interp->exePath = exePath;
    interp->gc = gc;

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
    interp->globalCtx = ctx;
    interp->modules = modules;
    interp->exePath = exePath;
    interp->gc = gc;

    AdkValue* value = interp->Evaluate(true);

    // interp->gc->clean();
    delete tree;

    return value;
  }

  void Interpreter::DefineGlobals(AdkContext* newCtx) {
    newCtx->Set("output", new AdkFunction(this, [](vector<AdkValue*> args, Interpreter* interp) {
      for (AdkValue* arg : args) {
        std::cout << arg->toString() << " ";
      }
      std::cout << "\n";
      
      return new AdkValue();
    }, "output"));
		
		newCtx->Set("input", new AdkFunction(this, [](vector<AdkValue*> args, Interpreter* interp) {
      for (AdkValue* arg : args) {
        std::cout << arg->toString();
				break;
      }
			std::string n;
      std::cin >> n;
      return interp->ConstructString(n);
    }, "input"));
		
		newCtx->Set("Number", new AdkFunction(this, [](vector<AdkValue*> args, Interpreter* interp) {
      std::string val = args[0]->toString();
      int num = 0;
      if (!isNumber(val)) {
        std::cout << "ERROR: \"Number\" requires a number as its first value" << std::endl;
        throw std::invalid_argument( "received non-number value" );
      } else {
        std::stringstream geek(val);
        geek >> num;
      }

      if (isDouble(val)) {
        return new AdkValue(std::stod(val));
      }

      return new AdkValue(num);
    }, "Number"));
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
    this->globalCtx = this->ctx;
  }

  Interpreter::Interpreter(AST* ast) {
    this->ast = ast;
    this->ctx = new AdkContext();
    this->globalCtx = this->ctx;
  }

  Interpreter::Interpreter(Interpreter& o) {
    this->ast = o.ast;
    this->ctx = o.ctx;
    this->globalCtx = o.globalCtx;
    this->gc = o.gc;
  }

}; // namespace Aardvark