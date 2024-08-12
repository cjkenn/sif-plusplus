#include "sif/Parser/parser.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/token.h"

using namespace sif;

std::unique_ptr<ParseResult> Parser::Parse() {
  std::vector<ASTNode> blocks;
  bool found_error = false;

  while (curr_token_.GetKind() != TokenKind::Eof) {
    auto result = decl();
    if (decl.has_value()) {
      blocks.push(decl.value());
    } else {
      found_error = true;
      auto error = decl.error();
      error.emit();
    }
  }

  auto head = std::make_unique<ProgramAST>(blocks);
  return std::make_unique<ParseResult>(std::optional<ASTNode>(head),
                                       found_error);
}
