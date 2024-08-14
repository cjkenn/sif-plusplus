#include "sif/Parser/symbol_table.h"
#include "sif/Parser/ast.h"
#include <optional>
#include <string>
#include <unordered_map>

using namespace sif;

std::optional<ASTNode> SymbolTable::Retrieve(std::string key) {
  int curr = curr_level_;
  for (;;) {
    if (curr < 0) {
      return std::nullopt;
    }

    if (table_.at(curr).contains(key)) {
      return std::make_optional(table_.at(curr)[key]);
    } else {
      curr--;
    }
  }
}
