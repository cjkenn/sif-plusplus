#include "sif/Parser/parser.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/token.h"

using namespace sif;

ParseFullResult Parser::Parse() {
  std::vector<std::unique_ptr<ASTNode>> blocks;
  bool found_error = false;

  while (curr_tkn_.GetKind() != TokenKind::Eof) {
    ParseCallResult result = decl();
    if (result.has_value()) {
      blocks.push_back(std::make_unique<ASTNode>(result.value()));
    } else {
      found_error = true;
      auto error = result.error();
      // TODO: emit errors
      // error.emit();
    }
  }

  ProgramAST program = ProgramAST(std::move(blocks));
  return ParseFullResult(std::make_unique<ASTNode>(program), found_error);
}

ParseCallResult decl() {}
