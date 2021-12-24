#include "aardvark.h"
#include "parser.h"
#include "gc.h"
#include "modules/adkFilesystem.hpp"
#include "modules/adkTime.hpp"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#else
#include "modules/adkSocket.hpp"
#endif

using namespace Aardvark;

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  if (argc < 2) return 0;
  // string info = readFile("./tests/main.adk");
  // Lexer lexer = Lexer();
  // std::vector<Token> tokens = lexer.tokenize(info);

  // std::cout << tokens.size() << std::endl;

  // for (auto token : tokens) {
  //   std::cout << "Token<" << ((int)token.type) << ", " << token.toString() << ">\n";
  // }

  // Parser parser = Parser(tokens);
  // AST* tree = parser.parse();
  
  // // Should be 'AST<6, Hello, >'
  // std::cout << tree->block[0]->right->toString() << std::endl;
  // // Should be 'AST<18, test>'
  // std::cout << tree->block[1]->toString() << std::endl;
  // // Should be 'AST<17, output>'
  // std::cout << tree->block[1]->block[0]->toString() << std::endl;
  // // Should be 'AST<13, x>'
  // std::cout << tree->block[2]->args[0]->toString() << std::endl;
  // // Should be 'AST<17, test>'
  // std::cout << tree->block[3]->toString() << std::endl;
  
  fs::path executablePath = argv[0];
  fs::path executableFullPath = fs::current_path() / fs::absolute(executablePath);
  fs::path adkFile = argv[1];
  fs::path fullFile = fs::current_path() / fs::absolute(adkFile);

  Interpreter* interpreter = new Interpreter();
  interpreter->gc = new AdkGC();
  interpreter->exePath = executableFullPath.string();
  initAdkFS(interpreter);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#else
  interpreter->defineModule("socket", initSocketModule(interpreter));
	interpreter->defineModule("time", initTimeModule(interpreter));
#endif
//Oh, thats how it works
  interpreter->EvaluateFile(fullFile.string());

  delete interpreter->ctx;
  delete interpreter->gc;
  delete interpreter;
  
  return 0;
}