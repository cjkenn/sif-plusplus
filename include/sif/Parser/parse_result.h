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
    ast = std::nullopt;
    contains_error = false;
  };
  ~ParseResult(){};

  std::optional<std::unique_ptr<ASTNode>> ast;
  bool contains_error;
  std::vector<ParseError> errors;

private:
};
} // namespace sif
