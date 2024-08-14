#pragma once

#include "sif/Parser/lexer.h"
#include "sif/Parser/parse_error.h"
#include "sif/Parser/parse_result.h"
#include "sif/Parser/symbol_table.h"
#include "sif/Parser/token.h"
#include <memory>

namespace sif {
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
  ParseCallResult decl();

  const size_t FN_PARAM_MAX_LEN = 64;

  std::unique_ptr<Lexer> lexer_;
  std::unique_ptr<SymbolTable> symtab_;
  Token curr_tkn_;
  std::vector<ParseError> errors;
  bool should_check_sym_tab_;
};
} // namespace sif
