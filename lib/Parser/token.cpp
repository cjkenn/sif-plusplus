#include "sif/Parser/token.h"

using namespace sif;

std::string Token::GetName() {
  switch (kind_) {
  case TokenKind::StringLiteral:
    return GetStringLit().value();
  case TokenKind::Identifier:
    return GetIdentLit().value();
  default:
    return "";
  }
}

float Token::GetNumber() {
  switch (kind_) {
  case TokenKind::NumberLiteral:
    return stof(GetNumLit().value());
  default:
    return 0.0;
  }
}
