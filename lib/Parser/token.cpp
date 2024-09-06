#include "sif/Parser/token.h"
#include <cassert>
#include <iostream>

using namespace sif;

std::string Token::GetName() {
  switch (kind_) {
  case TokenKind::StringLiteral:
    assert(Token::GetStringLit().has_value() && "String literal token should "
                                                "contain a value when "
                                                "attempting to get name!");
    return Token::GetStringLit().value();
  case TokenKind::Identifier:
    assert(Token::GetIdentLit().has_value() && "Identifier literal token "
                                               "should contain a value when "
                                               "attempting to get name!");
    return Token::GetIdentLit().value();
    // TODO: include reserved words here?
  default:
    assert(false && "GetName should not be called on a token with no name.");
    return "";
  }
}

float Token::GetNumber() {
  switch (kind_) {
  case TokenKind::NumberLiteral:
    return stof(GetNumLit().value_or("0"));
  default:
    return 0.0;
  }
}
