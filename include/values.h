#include "./datastructs/datatypes.h"

namespace Aardvark {

  using std::string;
  using std::vector;
  using std::map;

  class AdkContext;
  class Interpreter;

  class AdkValue {
    public:
    DataTypes type;
    void* data;

    bool returnedVal = false;

    AdkValue();
    AdkValue(DataTypes type);
    AdkValue(string val);
    AdkValue(int val);
    AdkValue(double val);

    AdkValue* operator+(AdkValue& val);
    AdkValue* operator-(AdkValue& val);
    AdkValue* operator*(AdkValue& val);
    AdkValue* operator/(AdkValue& val);

    string toString();

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
      return variables[name] = Variable(name, val);
    }

    Variable Lookup(string name) {
      bool hasVariable = HasVar(name);
      if ((!hasVariable && parent == nullptr) || (!hasVariable && parent == this)) {
        throw "Unknown Variable"; // Todo: Fix 'Error'
      } else if (!hasVariable) {
        return Lookup(name);
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

  extern "C" typedef AdkValue* (*NativeFunction)(vector<AdkValue*> args, Interpreter* interp);

  class AdkFunction : public AdkValue {
    public:
    Interpreter* interpreter = nullptr;

    NativeFunction func = nullptr;
    AST* info = nullptr;

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
    AdkObject* thisObj;
    Interpreter* interpreter = nullptr;

    AdkClass();
    AdkClass(Interpreter* interp, string className);
    AdkClass(Interpreter* interp, string className, AdkObject* thisObj);

    AdkValue* NewInstance(vector<AST*> args);
    AdkValue* NewInstance(vector<AdkValue*> args);
  };

}; // namespace Aardvark