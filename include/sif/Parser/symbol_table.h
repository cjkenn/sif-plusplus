#pragma once

#include "sif/Parser/ast.h"
#include <memory>
#include <optional>
#include <unordered_map>

namespace sif {
enum class SymbolKind { Fn, Var };
typedef std::unordered_map<std::string, ASTNode> Scope;

class SymbolTable {
public:
  SymbolTable() { curr_level_ = 0; }

  ~SymbolTable() {}

  void InitScope() {
    curr_level_++;
    Scope next;
    table_.push_back(next);
  }

  void CloseScope() { curr_level_--; }
  bool constexpr IsGlobal() { return curr_level_ == 0; }
  int Level() const { return curr_level_; }
  bool Contains(std::string key) { return Retrieve(key).has_value(); }
  void Store(std::string key, ASTNode ast) {
    table_.at(curr_level_).insert({key, ast});
  }

  std::optional<ASTNode> Retrieve(std::string key);

private:
  int curr_level_;
  std::vector<Scope> table_;
};
} // namespace sif
