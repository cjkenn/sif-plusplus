#include "sif/Parser/parser.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/token.h"
#include <cassert>
#include <memory>
#include <optional>

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
  auto lb = match(TokenKind::LeftBrace);
  if (lb.has_value()) {
    return std::make_unique<ParseCallResult>(lb.value());
  }

  std::vector<ASTPtr> decls;

  symtab_->InitScope();
  if (bindings.has_value()) {
    for (auto node : bindings.value()) {
      if (node->GetKind() == ASTKind::LiteralExpr) {
        auto pe = dynamic_cast<LiteralExprAST *>(node.get());
        symtab_->Store(pe->lit_tkn_.GetName(), *node);
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

  auto rb = match(TokenKind::RightBrace);
  if (rb.has_value()) {
    return std::make_unique<ParseCallResult>(rb.value());
  }

  int lvl = symtab_->Level();
  symtab_->CloseScope();
  BlockAST block = BlockAST(std::move(decls), lvl);
  return std::make_unique<ParseCallResult>(std::make_unique<ASTNode>(block));
}

ParseCallResultPtr Parser::var_decl() {
  std::optional<ParseError> next_var = match(TokenKind::Var);
  if (next_var.has_value()) {
    return std::make_unique<ParseCallResult>(next_var.value());
  }

  std::optional<Token> maybe_ident_tkn = match_ident();
  if (!maybe_ident_tkn.has_value()) {
    auto ident_err = add_error(ParseErrorKind::ExpectedIdent);
    return std::make_unique<ParseCallResult>(ident_err);
  }

  auto ident_tkn = maybe_ident_tkn.value();

  switch (curr_tkn_.GetKind()) {
  case TokenKind::Equal: {
    auto eq = match(TokenKind::Equal);
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
      auto sc = match(TokenKind::Semicolon);

      if (sc.has_value()) {
        return std::make_unique<ParseCallResult>(sc.value());
      } else {
        rhs = std::move(assign_result->ast());
      }
    }

    ASTPtr node = std::make_unique<VarDeclAST>(
        std::make_unique<Token>(ident_tkn), symtab_->IsGlobal(),
        std::make_optional(std::move(rhs)));

    symtab_->Store(ident_tkn.GetName(), *node.get());
    return std::make_unique<ParseCallResult>(std::move(node));
  }
  case TokenKind::Semicolon: {
    std::optional<ParseError> sc = match(TokenKind::Semicolon);
    if (sc.has_value()) {
      return std::make_unique<ParseCallResult>(sc.value());
    }

    ASTPtr node = std::make_unique<VarDeclAST>(
        std::make_unique<Token>(ident_tkn), symtab_->IsGlobal(), std::nullopt);
    symtab_->Store(ident_tkn.GetName(), *node.get());

    return std::make_unique<ParseCallResult>(std::move(node));
  }
  default:
    auto err = add_error(ParseErrorKind::InvalidToken);
    return std::make_unique<ParseCallResult>(err);
  }
}

ParseCallResultPtr Parser::fn_decl() {}
ParseCallResultPtr Parser::table_decl(Token ident_tkn) {}
ParseCallResultPtr Parser::array_decl(Token ident_tkn) {}

/**
   Parses an expression. Because the grammar encodes precedence, we must call
   each expression parsing method in the order starting from most general
   (lowest precedence) to most specific (highest precedence). Note that we
   recurse to higher precedence expressions first in each expression parse
   method, which is why we start with the lowest precedence instead of the
   highest.

     Precedence roughly follows this table:
     | = Assignment       | <- Lowest precedence
     | ||                 |
     | &&                 |
     | ==, !=             |
     | >, <, >=, <=       |
     | +, -               |
     | *, /               |
     | %                  |
     | -, ! Unary         |
     | Literals           | <- Highest precedence

     expr ::= assignexpr ;
 */
ParseCallResultPtr Parser::expr() { return assign_expr(); }

ParseCallResultPtr Parser::assign_expr() {
  auto ast = or_expr();
  if (ast->has_error()) {
    return ast;
  }

  if (curr_tkn_.GetKind() == TokenKind::Equal) {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());

    auto eq_match = match(TokenKind::Equal);
    if (eq_match.has_value()) {
      return std::make_unique<ParseCallResult>(eq_match.value());
    }

    auto rhs_result = assign_expr();
    if (rhs_result->has_error()) {
      return rhs_result;
    }

    auto rhs = rhs_result->ast();
    if (rhs->GetKind() == ASTKind::LiteralExpr) {
      auto primary_expr_ast = dynamic_cast<LiteralExprAST *>(rhs.get());
      Token tkn = primary_expr_ast->lit_tkn_;

      if (tkn.GetKind() == TokenKind::Identifier) {
        auto maybe_sym = symtab_->Retrieve(tkn.GetName());
        if (!maybe_sym.has_value()) {
          return std::make_unique<ParseCallResult>(
              add_error(ParseErrorKind::UndeclaredSymbol));
        }
        auto variable = maybe_sym.value();
        ASTPtr node = std::make_unique<VarAssignAST>(tkn, symtab_->IsGlobal(),
                                                     std::move(rhs));
        return std::make_unique<ParseCallResult>(std::move(node));
      } else {
        return std::make_unique<ParseCallResult>(
            add_error(ParseErrorKind::InvalidAssign));
      }
    } else if (rhs->GetKind() == ASTKind::ArrayAccess) {
      auto array_access_ast = dynamic_cast<ArrayAccessAST *>(rhs.get());
      ASTPtr node = std::make_unique<ArrayMutExprAST>(
          tkn, std::move(array_access_ast->index_), std::move(rhs));
      return std::make_unique<ParseCallResult>(std::move(node));
    } else {
      return std::make_unique<ParseCallResult>(
          add_error(ParseErrorKind::InvalidAssign));
    }
  } else {
    return std::make_unique<ParseCallResult>(
        add_error(ParseErrorKind::InvalidAssign));
  }
}

ParseCallResultPtr Parser::or_expr() {
  auto ast = and_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::and_expr() {
  auto ast = equality_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::equality_expr() {
  auto ast = compare_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::compare_expr() {
  auto ast = add_or_sub_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::add_or_sub_expr() {
  auto ast = mul_or_div_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::mul_or_div_expr() {
  auto ast = modulo_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::modulo_expr() {
  auto ast = unary_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::unary_expr() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::Bang:
  case TokenKind::Minus: {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());
    consume();
    auto rhs = unary_expr();
    if (rhs->has_error()) {
      return rhs;
    }
    ASTPtr unary_ast = rhs->ast();
    ASTPtr node = std::make_unique<UnaryExprAST>(tkn, std::move(unary_ast));
    return std::make_unique<ParseCallResult>(std::move(node));
  }
  default:
    return fn_call_expr();
  }
}

ParseCallResultPtr Parser::fn_call_expr() {
  auto ast = literal_expr();
  if (ast->has_error()) {
    return ast;
  }
}

ParseCallResultPtr Parser::group_expr() {
  auto has_paren = match(TokenKind::LeftParen);
  if (has_paren.has_value()) {
    return std::make_unique<ParseCallResult>(has_paren.value());
  }

  auto result = expr();

  has_paren = match(TokenKind::RightParen);
  if (has_paren.has_value()) {
    return std::make_unique<ParseCallResult>(has_paren.value());
  }

  return result;
}

// Parse a literal expression. Primary refers to either primitive types/values.
// This roughly corresponds to:
// 1. Number literals
// 2. String literals
// 3. Boolean literals
// 4. Identifiers
// 5. Parens, indicating a grouped expression.
//
// primary  ::= NUMBER |
//              STRING |
//              TRUE   |
//              FALSE  |
//              IDENT  |
//              groupexpr ;
ParseCallResultPtr Parser::literal_expr() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::StringLiteral:
  case TokenKind::NumberLiteral:
  case TokenKind::True:
  case TokenKind::False: {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());
    ASTPtr node = std::make_unique<LiteralExprAST>(tkn);
    consume();
    return std::make_unique<ParseCallResult>(std::move(node));
  }
  case TokenKind::Identifier: {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());
    if (check_symtab_for_ident_) {
      if (!symtab_->Contains(tkn.GetName())) {
        // TODO: check for std lib function call here
        auto err = add_error(ParseErrorKind::UndeclaredSymbol);
        consume();
        return std::make_unique<ParseCallResult>(err);
      }
    }

    ASTPtr node = std::make_unique<LiteralExprAST>(tkn);
    consume();
    return std::make_unique<ParseCallResult>(std::move(node));
  }
  case TokenKind::LeftParen:
    return group_expr();
  case TokenKind::At: {
    auto has_at = match(TokenKind::At);
    if (has_at.has_value()) {
      return std::make_unique<ParseCallResult>(has_at.value());
    }
    return fn_call_expr();
  }
  default: {
    auto err = add_error(ParseErrorKind::InvalidToken);
    consume();
    return std::make_unique<ParseCallResult>(err);
  }
  }
}

ParseCallResultPtr Parser::if_stmt() {}
ParseCallResultPtr Parser::for_stmt() {}
ParseCallResultPtr Parser::ret_stmt() {}
ParseCallResultPtr Parser::expr_stmt() {
  auto node = expr();
  if (node->has_error()) {
    return node;
  }

  auto has_semi = match(TokenKind::Semicolon);
  if (has_semi.has_value()) {
    return std::make_unique<ParseCallResult>(has_semi.value());
  }

  ASTPtr result = std::make_unique<ExprStmtAST>(std::move(node->ast()));
  return std::make_unique<ParseCallResult>(std::move(result));
}

std::optional<Token> Parser::match_ident() {
  switch (curr_tkn_.GetKind()) {
  case TokenKind::Identifier: {
    auto tkn =
        Token(curr_tkn_.GetKind(), curr_tkn_.GetPos(), curr_tkn_.GetLine());

    assert(curr_tkn_.GetIdentLit().has_value() &&
           "current identifier token should contain an identifier name!");

    tkn.SetIdentLit(curr_tkn_.GetIdentLit().value());
    consume();
    return std::make_optional<Token>(tkn);
  }
  default:
    add_error(ParseErrorKind::InvalidIdent);
    return std::nullopt;
  }
}

std::optional<ParseError> Parser::match(TokenKind kind) {
  if (curr_tkn_.GetKind() == kind) {
    consume();
    return std::nullopt;
  }

  // TODO: real errors
  auto err = ParseError(ParseErrorKind::TokenMismatch, curr_tkn_.GetLine(),
                        curr_tkn_.GetPos());
  return std::make_optional<ParseError>(err);
}

ParseError Parser::add_error(ParseErrorKind kind) {
  auto err = ParseError(kind, curr_tkn_.GetLine(), curr_tkn_.GetPos());
  errors_.push_back(err);
  return err;
}

void Parser::consume() { curr_tkn_ = lexer_->Lex(); }
