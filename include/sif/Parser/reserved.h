#pragma once

#include "sif/Parser/token.h"
#include <string>
#include <unordered_map>

namespace sif {
inline std::unordered_map<std::string, TknTy> get_reserved_words() {
  std::unordered_map<std::string, TknTy> reserved = {
      {"if", TKN_IF},       {"elif", TKN_ELIF},   {"else", TKN_ELSE},
      {"var", TKN_VAR},     {"fn", TKN_FN},       {"return", TKN_RET},
      {"table", TKN_TABLE}, {"array", TKN_ARRAY}, {"for", TKN_FOR},
      {"in", TKN_IN},       {"true", TKN_TRUE},   {"false", TKN_FALSE},
  };
  return reserved;
}
} // namespace sif
