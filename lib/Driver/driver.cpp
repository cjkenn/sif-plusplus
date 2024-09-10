#include "sif/Driver/driver.h"
#include "sif/Parser/lexer.h"
#include "sif/Parser/parser.h"
#include "sif/Parser/symbol_table.h"
#include "sif/Parser/token.h"
#include <cassert>
#include <iostream>

using namespace sif;

void Driver::run() {
  Lexer l = Lexer(filename_);
  SymbolTable symtab = SymbolTable();
  Parser parser =
      Parser(std::make_unique<Lexer>(l), std::make_unique<SymbolTable>(symtab));

  auto result = parser.Parse();
  assert(result.contains_error_ == false);
  assert(result.ast_ != nullptr);
  assert(result.ast_.GetKind() == ASTKind::Program);
  std::cout << "Parsing successful\n";
}
