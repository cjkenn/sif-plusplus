#pragma once

#include "token.h"
#include <string>

namespace sif {
class Lexer {
public:
  Lexer(){};
  Lexer(std::string source){};
  ~Lexer(){};

  Token Next();

private:
  void advance();
  Token lex_string();
};
} // namespace sif
