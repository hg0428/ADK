#pragma once
#include "./datastructs/datatypes.h"
#include <functional>

namespace Aardvark {

  using std::string;
  using std::vector;
  using std::map;
  using std::int64_t;

  class AdkContext;
  class Interpreter;

  class AdkValue {
    public:
    DataTypes type;
    void* data;

    bool returned = false;
    bool continued = false;
    bool breaked = false;

    bool isAccessible = false;

    AdkValue();
    AdkValue(DataTypes type);
    AdkValue(string val);
    AdkValue(int64_t val);
    AdkValue(int val);
    AdkValue(double val);
    AdkValue(bool val);

    AdkValue* operator+(AdkValue& val);
    AdkValue* operator-(AdkValue& val);
    AdkValue* operator*(AdkValue& val);
    AdkValue* operator/(AdkValue& val);
    AdkValue* operator%(AdkValue& val);
    AdkValue* operator==(AdkValue& val);
    AdkValue* operator!=(AdkValue& val);
    AdkValue* operator>=(AdkValue& val);
    AdkValue* operator<=(AdkValue& val);
    AdkValue* operator>(AdkValue& val);
    AdkValue* operator<(AdkValue& val);
    AdkValue* operator&&(AdkValue& val);
    AdkValue* operator||(AdkValue& val);

    bool toBool();
    int toInt();
    double toDouble();
    string toString();

    bool isNone() {
      return (
        type == DataTypes::None
      );
    }

    template<typename T>
    T cast() {
      return *((T*)(data)); // Maybe reinterpret_cast?
    }

    private:
  };

  class Variable {
    public:
    string name;
    AdkValue* value;

    Variable();
    Variable(string name);
    Variable(string name, AdkValue* value);

    private:
  };

  class AdkContext {
    public:
    AdkContext* parent = nullptr;

    map<string, Variable> variables;

    AdkContext();
    AdkContext(AdkContext* parent);

    bool HasVar(string name) {
      return variables.find(name) != variables.end();
    }

    Variable Set(string name, AdkValue* val) {
      bool hasVariable = HasVar(name) || (name == "this");

      if (hasVariable || parent == nullptr) {
        return variables[name] = Variable(name, val);
      } else if (parent != nullptr) {
        return parent->Set(name, val);
      }

      return variables[name] = Variable(name, val);
    }

    Variable Lookup(string name) {
      bool hasVariable = HasVar(name);
      if ((!hasVariable && parent == nullptr) || (!hasVariable && parent == this)) {
        std::cout << "Error: Could not get variable '" << name << "' as it is not defined." << std::endl;
        throw "Unknown Variable"; // Todo: Fix 'Error'
      } else if (!hasVariable) {
        return parent->Lookup(name);
      }

      return variables[name];
    }

    Variable Get(string name) {
      return Lookup(name);
    }

    AdkContext* Extend() {
      return new AdkContext(this);
    }

    private:
  };

  // extern "C" typedef AdkValue* (*NativeFunction)(vector<AdkValue*> args, Interpreter* interp);

  extern "C" typedef std::function<AdkValue*(vector<AdkValue*>, Interpreter*)> NativeFunction;

  class AdkFunction : public AdkValue {
    public:
    Interpreter* interpreter = nullptr;

    NativeFunction func = nullptr;
    AST* info = nullptr;
    bool isStatic = false;

    string name = ""; // Name of function

    AdkFunction(Interpreter* interp, AST* exp);
    AdkFunction(Interpreter* interp, NativeFunction func, string name);

    AdkValue* Call(vector<AST*> args, AdkValue* thisObj);
    AdkValue* Call(vector<AdkValue*> args, AdkValue* thisObj);
  };

  class AdkObject: public AdkValue {
    public:
    string objType = "None";
    std::map<string, Variable> props;

    AdkObject();
    AdkObject(string o);

    bool isNone();

    bool HasProp(string name) {
      return props.find(name) != props.end();
    }

    Variable Set(string name, AdkValue* val) {
      return props[name] = Variable(name, val);
    }

    Variable Get(string name) {
      if (!HasProp(name)) {
        return Variable("", new AdkValue());
      }

      return props[name];
    }

    AdkObject* Copy() {
      AdkObject* obj = new AdkObject(objType);
      obj->props = props;

      return obj;
    }
  };

  class AdkClass : public AdkValue {
    public:
    string className = "";
    AdkObject* staticValues;

    AdkObject* thisObj;
    Interpreter* interpreter = nullptr;

    AdkClass();
    AdkClass(Interpreter* interp, string className);
    AdkClass(Interpreter* interp, string className, AdkObject* thisObj);

    // For accessing static functions and properties
    bool HasStaticVal(string name) {
      return staticValues->HasProp(name);
    }
    Variable Set(string name, AdkValue* value) {
      return staticValues->Set(name, value);
    }
    Variable Get(string name) {
      return staticValues->Get(name);
    }

    AdkValue* NewInstance(vector<AST*> args);
    AdkValue* NewInstance(vector<AdkValue*> args);
  };

}; // namespace Aardvark