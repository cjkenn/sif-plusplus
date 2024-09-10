#include "sif/Parser/lexer.h"
#include "sif/Parser/reserved.h"
#include "sif/Parser/token.h"
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace sif;

Lexer::Lexer(std::string filename) {
  std::ifstream infile;
  infile.open(filename);

  assert(infile.is_open() && "Failed to open file!");

  std::vector<std::string> buffer;
  std::string line;
  while (getline(infile, line)) {
    buffer.push_back(line.append("\n"));
  }

  infile.close();

  buffer_ = buffer;
  curr_char_ = std::optional<char>{buffer[0][0]};
  curr_pos_ = 0;
  curr_line_ = 0;
  reserved_words_ = get_reserved_words();
};

Token Lexer::Lex() {
  if (!curr_char_.has_value()) {
    return Token(TokenKind::Eof, curr_pos_, curr_line_);
  }

  skip_whitespace();
  if (!curr_char_.has_value()) {
    return Token(TokenKind::Eof, curr_pos_, curr_line_);
  }

  while (curr_char_ == '#') {
    // to handle comments, we move to the next line and advance
    // the current char
    next_line();
    advance();
    if (!curr_char_.has_value()) {
      return Token(TokenKind::Eof, curr_pos_, curr_line_);
    }
  }

  assert(curr_char_.has_value() &&
         "Error: current character should have value");

  char curr = curr_char_.value();
  if (isdigit(curr)) {
    return lex_num_lit();
  } else if (isalpha(curr)) {
    return lex_ident();
  }

  switch (curr) {
  case '(':
    return consume(TokenKind::LeftParen);
  case ')':
    return consume(TokenKind::RightParen);
  case '{':
    return consume(TokenKind::LeftBrace);
  case '}':
    return consume(TokenKind::RightBrace);
  case '[': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '[') {
      Token result = consume(TokenKind::DoubleLeftBracket);
      advance();
      return result;
    } else {
      return consume(TokenKind::LeftBracket);
    }
  }
  case ']': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == ']') {
      Token result = consume(TokenKind::DoubleRightBracket);
      advance();
      return result;
    } else {
      return consume(TokenKind::RightBracket);
    }
  }
  case ';':
    return consume(TokenKind::Semicolon);
  case '.':
    return consume(TokenKind::Period);
  case ',':
    return consume(TokenKind::Comma);
  case '+':
    return consume(TokenKind::Plus);
  case '-':
    return consume(TokenKind::Minus);
  case '*':
    return consume(TokenKind::Star);
  case '%':
    return consume(TokenKind::Percent);
  case '@':
    return consume(TokenKind::At);
  case '/': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '/') {
      while (curr_char_.value() != '\n') {
        advance();
      }
      return Lex();
    } else {
      return consume(TokenKind::Slash);
    }
  }
  case '=': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TokenKind::EqualEqual);
      advance();
      return result;
    } else if (next.has_value() && next.value() == '>') {
      Token result = consume(TokenKind::EqualArrow);
      advance();
      return result;
    } else {
      return consume(TokenKind::Equal);
    }
  }
  case '<': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TokenKind::LessThanEqual);
      advance();
      return result;
    } else {
      return consume(TokenKind::LessThan);
    }
  }
  case '>': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TokenKind::GreaterThanEqual);
      advance();
      return result;
    } else {
      return consume(TokenKind::GreaterThan);
    }
  }
  case '!': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TokenKind::BangEqual);
      advance();
      return result;
    } else {
      return consume(TokenKind::Bang);
    }
  }
  case '&': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '&') {
      Token result = consume(TokenKind::DoubleAmpersand);
      advance();
      return result;
    } else {
      return consume(TokenKind::Ampersand);
    }
  }
  case '|': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '|') {
      Token result = consume(TokenKind::DoublePipe);
      advance();
      return result;
    } else {
      return consume(TokenKind::Pipe);
    }
  }
  case '"':
    return lex_str_lit();
  default:
    return Token(TokenKind::Eof, 0, 0);
  }
}

Token Lexer::lex_str_lit() {
  std::string literal;
  int str_start = curr_pos_;
  int str_line = curr_line_;

  advance();

  while (!finished()) {
    // TODO: if no value here we need to return an error.
    assert(curr_char_.has_value() && "current char should have a value!");
    switch (curr_char_.value()) {
    case '"': {
      Token next = consume_str_lit(literal, str_start, str_line);
      return next;
    }
    default: {
      literal += curr_char_.value();
      advance();
    }
    }
  }

  // if we get here, we have no characters left to lex
  // but the string literal is unterminated.
  // TODO: collect and emit an error here
  return Token(TokenKind::Eof, 0, 0);
}

Token Lexer::lex_num_lit() {
  std::string literal;
  int num_start = curr_pos_;
  int num_line = curr_line_;

  bool dot_allowed = true;
  while (!finished() && curr_char_.has_value() &&
         (isdigit(curr_char_.value()) || curr_char_.value() == '.')) {
    if (curr_char_.value() == '.') {
      if (!dot_allowed) {
        // TODO: proper error here
        assert(false && "incorrectly formatted number!");
      }
      dot_allowed = false;
    }
    literal += curr_char_.value();
    advance();
  }

  return consume_num_lit(literal, num_start, num_line);
}

Token Lexer::lex_ident() {
  std::string literal;
  int ident_start = curr_pos_;
  int ident_line = curr_line_;

  while (!finished() && curr_char_.has_value() &&
         std::isalpha(curr_char_.value())) {
    literal += curr_char_.value();
    advance();
  }

  if (reserved_words_.contains(literal)) {
    TokenKind curr_type = reserved_words_[literal];
    Token next = Token(curr_type, ident_start, ident_line);
    next.SetIdentLit(literal);
    return next;
  } else {
    Token next = Token(TokenKind::Identifier, ident_start, ident_line);
    next.SetIdentLit(literal);
    return next;
  }
}

Token Lexer::consume_str_lit(std::string str, int pos, int line) {
  Token tkn = Token(TokenKind::StringLiteral, pos, line);
  tkn.SetStringLit(str);
  advance();
  return tkn;
}

Token Lexer::consume_num_lit(std::string num, int pos, int line) {
  Token tkn = Token(TokenKind::NumberLiteral, pos, line);
  tkn.SetNumberLit(num);
  advance();
  return tkn;
}

Token Lexer::consume(TokenKind kind) {
  Token tkn = Token(kind, curr_pos_, curr_line_);
  advance();
  return tkn;
}

std::optional<char> Lexer::peek() {
  if (curr_pos_ >= buffer_[curr_line_].size() - 1) {
    return std::nullopt;
  }

  return std::optional<char>{buffer_[curr_line_][curr_pos_ + 1]};
}

void Lexer::advance() {
  bool on_new_line = curr_char_ == '\n';
  if (curr_pos_ == buffer_[curr_line_].size() - 1 || on_new_line) {
    next_line();
  } else {
    curr_pos_++;
  }

  if (finished()) {
    curr_char_ = std::nullopt;
  } else {
    curr_char_ = std::optional<char>{buffer_[curr_line_][curr_pos_]};
  }
}

void Lexer::next_line() {
  curr_line_++;
  curr_pos_ = 0;
}

void Lexer::skip_whitespace() {
  while (curr_char_.has_value() && std::isspace(curr_char_.value())) {
    advance();
  }
}
