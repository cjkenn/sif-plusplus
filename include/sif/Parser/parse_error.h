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

  void Emit() { std::cerr << "sif: Parse error - " << error_to_msg() << "\n"; }

  ~ParseError() {}

private:
  std::string error_to_msg();

  int line_;
  int pos_;
  ParseErrorKind kind_;
};

} // namespace sif
