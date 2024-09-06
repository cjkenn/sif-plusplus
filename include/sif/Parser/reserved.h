#pragma once

#include "sif/Parser/token.h"
#include <set>
#include <string>
#include <unordered_map>

namespace sif {
inline std::unordered_map<std::string, TokenKind> get_reserved_words() {
  std::unordered_map<std::string, TokenKind> reserved = {
      {"if", TokenKind::If},       {"elif", TokenKind::ElIf},
      {"else", TokenKind::Else},   {"var", TokenKind::Var},
      {"fn", TokenKind::Fn},       {"return", TokenKind::Ret},
      {"table", TokenKind::Table}, {"array", TokenKind::Array},
      {"for", TokenKind::For},     {"in", TokenKind::In},
      {"true", TokenKind::True},   {"false", TokenKind::False},
  };
  return reserved;
}

inline std::set<std::string> get_reserved_fns() {
  std::set<std::string> reserved = {"print", "range"};
  return reserved;
}

inline bool is_std_lib_fn(std::string name) {
  return get_reserved_fns().contains(name);
}

} // namespace sif
