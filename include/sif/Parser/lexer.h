#pragma once

#include "sif/Parser/token.h"
#include <optional>
#include <string>
#include <unordered_map>
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
  Token consume(TknTy ty);
  std::optional<char> peek();
  void skip_whitespace();
  void next_line();
  bool finished();

  std::vector<std::string> buffer_;
  std::optional<char> curr_char_;
  std::unordered_map<std::string, TknTy> reserved_words_;
  int curr_pos_;
  int curr_line_;
};
} // namespace sif
