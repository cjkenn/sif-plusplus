#include "sif/Driver/driver.h"
#include "sif/Parser/lexer.h"
#include "sif/Parser/token.h"
#include <iostream>

using namespace sif;

void Driver::run() {
  Lexer l = Lexer(filename_);
  Token first = l.Lex();
  std::cout << first.GetIdentLit().value();
}
