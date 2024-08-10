#pragma once

#include <optional>

namespace sif {
enum TknTy {
  // Single character tokens
  TKN_LPAREN,
  TKN_RPAREN,
  TKN_LBRACE,
  TKN_RBRACE,
  TKN_LBRACK,
  TKN_RBRACK,
  TKN_SEMICOLON,
  TKN_EQ,
  TKN_LT,
  TKN_GT,
  TKN_PERIOD,
  TKN_COMMA,
  TKN_BANG,
  TKN_PLUS,
  TKN_MINUS,
  TKN_STAR,
  TKN_SLASH,
  TKN_PERCENT,
  TKN_AMP,
  TKN_PIPE,
  TKN_AT,

  // Multi-character tokens
  TKN_EQEQ,
  TKN_LTEQ,
  TKN_GTEQ,
  TKN_EQARROW,
  TKN_BANGEQ,
  TKN_AMPAMP,
  TKN_PIPEPIPE,
  TKN_DOUBLELBRACK,
  TKN_DOUBLERBRACK,
  TKN_IDENT,
  TKN_STRLIT,
  TKN_NUMLIT,
  TKN_EOF,

  // Reserved word tokens
  TKN_IF,
  TKN_ELIF,
  TKN_ELSE,
  TKN_VAR,
  TKN_FN,
  TKN_RET,
  TKN_TABLE,
  TKN_ARRAY,
  TKN_FOR,
  TKN_IN,
  TKN_TRUE,
  TKN_FALSE
};

class Token {
public:
  Token(TknTy ty, int pos, int line) {
    ty_ = ty;
    pos_ = pos;
    line_ = line;
  }

  ~Token(){};

  TknTy GetTy() const { return ty_; }
  int GetPos() const { return pos_; }
  int GetLine() const { return line_; }
  std::optional<std::string> GetStringLit() const { return str_lit_; }
  std::optional<std::string> GetIdentLit() const { return ident_lit_; }
  std::optional<std::string> GetNumLit() const { return num_lit_; }
  void SetPos(int pos) { pos_ = pos; }
  void SetLine(int line) { line_ = line; }
  void SetStrLit(std::string lit) { str_lit_ = lit; }
  void SetIdentLit(std::string lit) { ident_lit_ = lit; }
  void SetNumLit(std::string lit) { num_lit_ = lit; }

private:
  TknTy ty_;
  int pos_;
  int line_;
  // TODO: this should be a class that extends an abstract class Token
  std::optional<std::string> str_lit_;
  std::optional<std::string> ident_lit_;
  std::optional<std::string> num_lit_;
};
} // namespace sif
