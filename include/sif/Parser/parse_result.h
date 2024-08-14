#pragma once

#include "sif/Parser/ast.h"
#include "sif/Parser/parse_error.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
class ParseFullResult {
public:
  ParseFullResult() {
    ast_ = nullptr;
    contains_error_ = false;
  };

  ParseFullResult(std::unique_ptr<ASTNode> ast, bool contains_error) {
    ast_ = std::move(ast);
    contains_error_ = contains_error;
  }

  ~ParseFullResult() {}

  std::unique_ptr<ASTNode> ast_;
  bool contains_error_;
  std::vector<ParseError> errors_;
};

class ParseCallResult {
public:
  ParseCallResult(std::unique_ptr<ASTNode> ast) {
    ast_ = std::move(ast);
    err_ = nullptr;
  }

  ParseCallResult(ParseError err) {
    err_ = std::make_unique<ParseError>(err);
    ast_ = nullptr;
  }

  ~ParseCallResult() {}

  bool has_value() { return ast_ != nullptr; }
  ASTNode value() { return *ast_; }
  ParseError error() { return *err_; }

private:
  // TODO: why would these be pointers?
  std::unique_ptr<ASTNode> ast_;
  std::unique_ptr<ParseError> err_;
};
} // namespace sif
