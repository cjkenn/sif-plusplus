#pragma once

#include "sif/Parser/ast.h"
#include "sif/Parser/parse_error.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
class ParseResult {
public:
  ParseResult() {
    ast_ = std::nullopt;
    contains_error_ = false;
  };

  ParseResult(std::optional<std::unique_ptr<ASTNode>> ast,
              bool contains_error) {
    ast_ = std::move(ast);
    contains_error_ = contains_error;
  }

  ~ParseResult() {}

  std::optional<std::unique_ptr<ASTNode>> ast_;
  bool contains_error_;
  std::vector<ParseError> errors_;

private:
};
} // namespace sif
