#pragma once

#include <memory>
#include <optional>
#include <type_traits>

namespace sif {
class Token;
typedef std::unique_ptr<Token> TokenPtr;

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

template <typename T>
std::ostream &operator<<(
    typename std::enable_if<std::is_enum<T>::value, std::ostream>::type &stream,
    const T &e) {
  return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

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
  std::optional<std::string> GetNumberLit() const { return num_lit_; }
  void SetPos(int pos) { pos_ = pos; }
  void SetLine(int line) { line_ = line; }
  void SetStringLit(std::string lit) {
    str_lit_ = std::make_optional<std::string>(lit);
  }
  void SetIdentLit(std::string lit) {
    ident_lit_ = std::make_optional<std::string>(lit);
  }
  void SetNumberLit(std::string lit) {
    num_lit_ = std::make_optional<std::string>(lit);
  }
  std::string GetName();
  float GetNumber();

private:
  TokenKind kind_;
  int pos_;
  int line_;
  // TODO: this should be a class that extends an abstract class Token
  std::optional<std::string> str_lit_;
  std::optional<std::string> ident_lit_;
  std::optional<std::string> num_lit_;
};

class TokenFactory {
public:
  static Token MakeIdentToken(int pos, int line, std::string ident) {
    Token t = Token(TokenKind::Identifier, pos, line);
    t.SetIdentLit(ident);
    return t;
  }

  static Token MakeStringLiteralToken(int pos, int line, std::string literal) {
    Token t = Token(TokenKind::StringLiteral, pos, line);
    t.SetStringLit(literal);
    return t;
  }

  static Token MakeNumberLiteralToken(int pos, int line, std::string literal) {
    Token t = Token(TokenKind::StringLiteral, pos, line);
    t.SetNumberLit(literal);
    return t;
  }
};
} // namespace sif
