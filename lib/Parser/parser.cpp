#include "sif/Parser/parser.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/token.h"
#include <memory>

using namespace sif;

ParseFullResult Parser::Parse() {
  std::vector<ASTPtr> blocks;
  bool found_error = false;

  while (curr_tkn_.GetKind() != TokenKind::Eof) {
    auto result = decl();
    if (result->has_ast()) {
      blocks.push_back(std::move(result->ast()));
    } else {
      found_error = true;
      auto error = result->error();
      // TODO: emit errors
      // error.emit();
    }
  }

  ProgramAST program = ProgramAST(std::move(blocks));
  return ParseFullResult(std::make_unique<ASTNode>(program), found_error);
}

ParseCallResultPtr Parser::decl() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::Var:
    return var_decl();
  case TokenKind::Fn:
    return fn_decl();
  default:
    return stmt();
  }
}

ParseCallResultPtr Parser::stmt() {
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

ParseCallResultPtr Parser::block(OptionalBlockBindings bindings) {
  auto lb = expect(TokenKind::LeftBrace);
  if (lb.has_value()) {
    return std::make_unique<ParseCallResult>(lb.value());
  }

  std::vector<ASTPtr> decls;

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
      if (result->has_ast()) {
        decls.push_back(std::move(result->ast()));
      } else {
        errors_.push_back(result->error());
      }
    }
  }

  auto rb = expect(TokenKind::RightBrace);
  if (rb.has_value()) {
    return std::make_unique<ParseCallResult>(rb.value());
  }

  int lvl = symtab_->Level();
  symtab_->CloseScope();
  BlockAST block = BlockAST(std::move(decls), lvl);
  return std::make_unique<ParseCallResult>(std::make_unique<ASTNode>(block));
}

ParseCallResultPtr Parser::var_decl() {
  auto expected = expect(TokenKind::Var);
  if (expected.has_value()) {
    return std::make_unique<ParseCallResult>(expected.value());
  }

  std::optional<Token> maybe_ident_tkn = match_ident();
  if (!maybe_ident_tkn.has_value()) {
    auto ident_err = add_error(ParseErrorKind::ExpectedIdent);
    return std::make_unique<ParseCallResult>(ident_err);
  }

  auto ident_tkn = maybe_ident_tkn.value();

  switch (curr_tkn_.GetKind()) {
  case TokenKind::Equal: {
    auto eq = expect(TokenKind::Equal);
    if (eq.has_value()) {
      return std::make_unique<ParseCallResult>(eq.value());
    }

    ASTPtr rhs = nullptr;
    if (curr_tkn_.GetKind() == TokenKind::LeftBracket) {
      rhs = std::move(array_decl(ident_tkn)->ast());
    } else if (curr_tkn_.GetKind() == TokenKind::DoubleLeftBracket) {
      rhs = std::move(table_decl(ident_tkn)->ast());
    } else {
      auto assign_result = expr();
      auto sc = expect(TokenKind::Semicolon);
      if (sc.has_value()) {
        return std::make_unique<ParseCallResult>(sc.value());
      } else {
        rhs = std::move(assign_result->ast());
      }
    }

    // TODO: continue with rhs and returning a var_decl here
    ASTPtr node = std::make_unique<VarDeclAST>(
        std::make_unique<Token>(ident_tkn), symtab_->IsGlobal(),
        std::make_optional(std::move(rhs)));

    // symtab_->Store(ident_tkn.GetName(), std::copy(node));
    // return std::make_unique<ParseCallResult>(node);
  }
  }
}

std::optional<Token> Parser::match_ident() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::Identifier: {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());
    consume();
    return std::make_optional<Token>(tkn);
  }
  default:
    add_error(ParseErrorKind::InvalidIdent);
    return std::nullopt;
  }
}

std::optional<ParseError> Parser::expect(TokenKind kind) {
  if (curr_tkn_.GetKind() == kind) {
    consume();
    return std::nullopt;
  }

  // TODO: real errors
  return std::make_optional<ParseError>(
      ParseErrorKind::TokenMismatch, curr_tkn_.GetLine(), curr_tkn_.GetPos());
}

ParseError Parser::add_error(ParseErrorKind kind) {
  auto err = ParseError(kind, curr_tkn_.GetLine(), curr_tkn_.GetPos());
  errors_.push_back(err);
  return err;
}

void Parser::consume() { curr_tkn_ = lexer_->Lex(); }
