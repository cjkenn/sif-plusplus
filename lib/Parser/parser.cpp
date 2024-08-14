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

ParseCallResult Parser::decl() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::Var:
    return var_decl();
  case TokenKind::Fn:
    return fn_decl();
  default:
    return stmt();
  }
}

ParseCallResult Parser::stmt() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::If:
    return if_stmt();
  case TokenKind::For:
    return for_stmt();
  case TokenKind::Ret:
    return ret_stmt();
  case TokenKind::LeftBrace:
    return block(std::nullopt);
  default:
    return expr_stmt();
  }
}

ParseCallResult Parser::block(OptionalBlockBindings bindings) {
  auto lb = expect(TokenKind::LeftBrace);
  if (lb.has_value()) {
    return ParseCallResult(lb.value());
  }

  std::vector<std::unique_ptr<ASTNode>> decls;

  // TODO: init symbol table scope
  symtab_->InitScope();
  if (bindings.has_value()) {
    for (auto node : bindings.value()) {
      if (node->GetKind() == ASTKind::PrimaryExpr) {
        auto pe = dynamic_cast<PrimaryExprAST *>(node.get());
        symtab_->Store(pe->token_.GetName(), *node);
      }
    }
  }

  for (;;) {
    if (curr_tkn_.GetKind() == TokenKind::RightBrace) {
      break;
    } else if (curr_tkn_.GetKind() == TokenKind::Eof) {
      break;
    } else {
      auto result = decl();
      if (result.has_value()) {
        decls.push_back(std::make_unique<ASTNode>(result.value()));
      } else {
        errors_.push_back(result.error());
      }
    }
  }

  auto rb = expect(TokenKind::RightBrace);
  if (rb.has_value()) {
    return ParseCallResult(rb.value());
  }

  // TODO: scope
  int lvl = symtab_->Level();
  symtab_->CloseScope();
  BlockAST block = BlockAST(std::move(decls), lvl);
  return ParseCallResult(std::make_unique<ASTNode>(block));
}

std::optional<ParseError> Parser::expect(TokenKind kind) {
  if (curr_tkn_.GetKind() == kind) {
    consume();
    return std::nullopt;
  }

  // TODO: real errors
  return std::make_optional<ParseError>();
}

void Parser::consume() { curr_tkn_ = lexer_->Lex(); }
