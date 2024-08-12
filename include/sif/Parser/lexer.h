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
  ~Lexer() {}

  Token Lex();

private:
  inline bool finished() { return curr_line_ >= buffer_.size(); }

  void advance();
  Token lex_str_lit();
  Token lex_num_lit();
  Token lex_ident();
  Token consume(TknTy ty);
  Token consume_str_lit(std::string str, int pos, int line);
  Token consume_num_lit(std::string num, int pos, int line);
  std::optional<char> peek();
  void skip_whitespace();
  void next_line();

  std::vector<std::string> buffer_;
  std::optional<char> curr_char_;
  std::unordered_map<std::string, TknTy> reserved_words_;
  int curr_pos_;
  int curr_line_;
};
} // namespace sif
