#include "aardvark.h"
#include "parser.h"
#include "modules/adkFilesystem.hpp"

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
  
  fs::path adkFile = argv[1];
  fs::path fullFile = fs::current_path() / fs::absolute(adkFile);

  Interpreter* interpreter = new Interpreter();
  initAdkFS(interpreter);

  interpreter->EvaluateFile(fullFile.string());
  
  return 0;
}