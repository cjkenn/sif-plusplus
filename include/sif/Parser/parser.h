#pragma once

#include "sif/Parser/lexer.h"
#include "sif/Parser/parse_error.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/symbol_table.h"
#include "sif/Parser/token.h"
#include <memory>
#include <optional>

namespace sif {
typedef std::optional<std::vector<std::shared_ptr<ASTNode>>>
    OptionalBlockBindings;

class Parser {
public:
  Parser(std::unique_ptr<Lexer> lexer, std::unique_ptr<SymbolTable> symtab)
      : curr_tkn_(TokenKind::Eof, 0, 0) {
    lexer_ = std::move(lexer);
    symtab_ = std::move(symtab);
    curr_tkn_ = lexer_->Lex();
  }

  ~Parser() {}
  ParseFullResult Parse();

private:
  const size_t FN_PARAM_MAX_LEN = 64;

  ParseCallResultPtr decl();
  ParseCallResultPtr stmt();
  ParseCallResultPtr expr();
  ParseCallResultPtr var_decl();
  ParseCallResultPtr fn_decl();
  ParseCallResultPtr table_decl(Token ident_tkn);
  ParseCallResultPtr array_decl(Token ident_tkn);

  ParseCallResultPtr if_stmt();
  ParseCallResultPtr for_stmt();
  ParseCallResultPtr ret_stmt();
  ParseCallResultPtr expr_stmt();

  ParseCallResultPtr block(OptionalBlockBindings bindings);

  std::optional<Token> match_ident();
  std::optional<ParseError> match(TokenKind kind);
  void consume();
  ParseError add_error(ParseErrorKind kind);

  std::unique_ptr<Lexer> lexer_;
  std::unique_ptr<SymbolTable> symtab_;
  Token curr_tkn_;
  std::vector<ParseError> errors_;
  bool should_check_sym_tab_;
};
} // namespace sif
