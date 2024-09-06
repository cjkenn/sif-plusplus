#pragma once

#include "ast.h"
#include "sif/Parser/ast.h"
#include "sif/Parser/parse_error.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
class ParseCallResult;
typedef std::unique_ptr<ParseCallResult> ParseCallResultPtr;

class ParseFullResult {
public:
  ParseFullResult() {
    ast_ = nullptr;
    contains_error_ = false;
  };

  ParseFullResult(ASTPtr ast, bool contains_error) {
    ast_ = std::move(ast);
    contains_error_ = contains_error;
  }

  ~ParseFullResult() {}

  ASTPtr ast_;
  bool contains_error_;
  std::vector<ParseError> errors_;
};

class ParseCallResult {
public:
  // it would be better to use a builder pattern here
  // instead of multiple constructors probably
  ParseCallResult(ASTPtr ast) {
    ast_ = std::move(ast);
    err_ = nullptr;
  }

  ParseCallResult(ParseError err) {
    err_ = std::make_unique<ParseError>(err);
    ast_ = nullptr;
  }

  ParseCallResult() {}

  ~ParseCallResult() {}

  bool has_ast() { return ast_ != nullptr; }
  bool has_error() { return err_ != nullptr; }
  // TODO: too much moving here? would prefer this to be a pointer
  // instead of a move
  ASTPtr ast() { return std::move(ast_); }
  ParseError error() { return *err_; }

private:
  // TODO: why would these be pointers?
  ASTPtr ast_;
  std::unique_ptr<ParseError> err_;
};

class ParseCallResultFactory {
public:
  static ParseCallResultPtr from_ast(ASTPtr node) {
    return std::make_unique<ParseCallResult>(std::move(node));
  }

  static ParseCallResultPtr from_err(ParseError err) {
    return std::make_unique<ParseCallResult>(err);
  }
};
} // namespace sif
