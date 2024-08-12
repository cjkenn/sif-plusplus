#pragma once

#include "sif/Parser/lexer.h"
#include "sif/Parser/parse_result.h"
#include <memory>

namespace sif {
class Parser {
public:
  Parser(){};
  ~Parser(){};
  std::unique_ptr<ParseResult> Parse();

private:
  std::unique_ptr<Lexer> lexer_;
};
} // namespace sif
