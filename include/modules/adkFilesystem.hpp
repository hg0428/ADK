#include "../aardvark.h"

using namespace Aardvark;

class Filesystem {
  public:
  static std::string readFile(std::string filename) {
    std::ifstream file(filename);

    std::string line;
    std::string data = "";
    bool isFirst = true;

    while (std::getline(file, line)) {
      if (isFirst) isFirst = false;
      else data += "\n";
      data += line;
    }

    file.close();

    return data;
  }

  static void writeFile(std::string filename, std::string data) {
    std::ofstream file(filename);

    file << data;

    file.close();
  }

  static void appendFile(std::string filename, std::string data) {
    std::string fileInfo = readFile(filename);
    fileInfo += data;

    std::ofstream file(filename);

    file << fileInfo;

    file.close();
  }

  static int fileSize(std::string filename) {
    return (int)std::filesystem::file_size(filename);
  }
};

AdkObject* createFileObject(Interpreter* i, std::string fileName) {
  AdkObject* obj = (AdkObject*)i->gc->addValue(new AdkObject("File"));
  obj->Set("filename", i->gc->addValue(new AdkValue(fileName)));

  obj->Set("read", i->gc->addValue(new AdkFunction(
    i,
    [](std::vector<AdkValue*> args, Interpreter* interp) {
      AdkObject* obj = (AdkObject*)interp->ctx->Get("this").value;
      std::string fileName = obj->Get("filename").value->toString();

      std::string fileData = Filesystem::readFile(fileName);

      return interp->ConstructString(fileData);

    }, "read"))
  );

  obj->Set("write", i->gc->addValue(new AdkFunction(
    i,
    [](std::vector<AdkValue*> args, Interpreter* interp) {
      std::string data = args[0]->toString();

      AdkObject* obj = (AdkObject*)interp->ctx->Get("this").value;
      std::string fileName = obj->Get("filename").value->toString();

      Filesystem::writeFile(fileName, data);

      return new AdkValue();

    }, "write"))
  );

  obj->Set("append", i->gc->addValue(new AdkFunction(
    i,
    [](std::vector<AdkValue*> args, Interpreter* interp) {
      std::string data = args[0]->toString();

      AdkObject* obj = (AdkObject*)interp->ctx->Get("this").value;
      std::string fileName = obj->Get("filename").value->toString();

      Filesystem::appendFile(fileName, data);

      return new AdkValue();

    }, "append"))
  );

  obj->Set("file_size", i->gc->addValue(new AdkFunction(
    i,
    [](std::vector<AdkValue*> args, Interpreter* interp) {
      AdkObject* obj = (AdkObject*)interp->ctx->Get("this").value;
      std::string fileName = obj->Get("filename").value->toString();

      double fileData = ((double)Filesystem::fileSize(fileName)) / 1024.0;

      return new AdkValue(fileData);

    }, "file_size"))
  );

  return obj;
}

void initAdkFS(Interpreter* mainInterp) {

  AdkContext* ctx = mainInterp->ctx;

  ctx->Set("open", mainInterp->gc->addValue(new AdkFunction(
    mainInterp,
    [](std::vector<AdkValue*> args, Interpreter* interp) {
      return (AdkValue*)createFileObject(interp, args[0]->toString());
    }, "open"))
  );
}