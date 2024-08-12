#pragma once

#include <optional>

namespace sif {
enum class TokenKind {
  // Single character tokens
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
  Semicolon,
  Equal,
  LessThan,
  GreaterThan,
  Period,
  Comma,
  Bang,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Ampersand,
  Pipe,
  At,

  // Multi-character tokens
  EqualEqual,
  LessThanEqual,
  GreaterThanEqual,
  EqualArrow,
  BangEqual,
  DoubleAmpersand,
  DoublePipe,
  DoubleLeftBracket,
  DoubleRightBracket,
  Identifier,
  StringLiteral,
  NumberLiteral,
  Eof,

  // Reserved word tokens
  If,
  ElIf,
  Else,
  Var,
  Fn,
  Ret,
  Table,
  Array,
  For,
  In,
  True,
  False
};

class Token {
public:
  Token(TokenKind kind, int pos, int line) {
    kind_ = kind;
    pos_ = pos;
    line_ = line;
  }

  ~Token() {}

  TokenKind GetKind() const { return kind_; }
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
  TokenKind kind_;
  int pos_;
  int line_;
  // TODO: this should be a class that extends an abstract class Token
  std::optional<std::string> str_lit_;
  std::optional<std::string> ident_lit_;
  std::optional<std::string> num_lit_;
};
} // namespace sif
