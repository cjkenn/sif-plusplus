#pragma once

#include "sif/Parser/token.h"
#include <optional>
#include <string>
#include <vector>

namespace sif {
class Lexer {
public:
  Lexer(std::string filename);
  ~Lexer(){};

  Token Lex();

private:
  void advance();
  Token lex_str();
  Token lex_num();
  Token lex_ident();
  Token consume();
  std::optional<char> peek();
  void skip_whitespace();
  void next_line();

  std::vector<std::string> buffer_;
  std::optional<char> currChar_;
  int currPos_;
  int currLine_;
};
} // namespace sif
