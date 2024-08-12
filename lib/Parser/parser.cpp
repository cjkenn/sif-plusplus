#include "sif/Parser/parser.h"
#include "sif/Parser/parse_result.h"

using namespace sif;

std::unique_ptr<ParseResult> Parser::Parse() {
  return std::make_unique<ParseResult>();
}
