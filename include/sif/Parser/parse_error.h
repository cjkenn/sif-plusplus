#pragma once

#include <iostream>
#include <string>

namespace sif {
enum class ParseErrorKind {
  InvalidIdent,
  InvalidToken,
  InvalidAssign,
  InvalidForStmt,
  InvalidIfStmt,
  TokenMismatch,
  FnParamCountExceeded,
  WrongFnParamCount,
  UndeclaredSymbol,
  UnassignedVar,
  ExpectedIdent
};

class ParseError {
public:
  ParseError(ParseErrorKind kind, int line, int pos) {
    kind_ = kind;
    line_ = line;
    pos_ = pos;
  };
  ~ParseError() {}

  ParseErrorKind Kind() { return kind_; }
  int Line() { return line_; }
  int Pos() { return pos_; }
  void Emit() { std::cerr << "sif: Parse error - " << error_to_msg() << "\n"; }

private:
  std::string error_to_msg();

  int line_;
  int pos_;
  ParseErrorKind kind_;
};

} // namespace sif
