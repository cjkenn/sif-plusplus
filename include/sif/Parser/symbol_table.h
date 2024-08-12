#pragma once

#include "sif/Parser/ast.h"

namespace sif {
enum class SymbolKind { Fn, Var };
typedef std::unordered_map<std::string, ASTNode> Scope;

class SymbolTable {
public:
  SymbolTable() { curr_level_ = 0; }

  ~SymbolTable() {}

private:
  size_t curr_level_;
  std::vector<Scope> table_;
};
} // namespace sif
