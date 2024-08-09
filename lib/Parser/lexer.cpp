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
    return Token(TKN_EOF, curr_pos_, curr_line_);
  }

  skip_whitespace();
  if (!curr_char_.has_value()) {
    return Token(TKN_EOF, curr_pos_, curr_line_);
  }

  while (curr_char_ == '#') {
    // to handle comments, we move to the next line and advance
    // the current char
    next_line();
    advance();
    if (!curr_char_.has_value()) {
      return Token(TKN_EOF, curr_pos_, curr_line_);
    }
  }

  assert(curr_char_.has_value() &&
         "Error: current character should have value");

  char curr = curr_char_.value();
  switch (curr) {
  case '(':
    return consume(TKN_LPAREN);
  case ')':
    return consume(TKN_RPAREN);
  case '{':
    return consume(TKN_LBRACE);
  case '}':
    return consume(TKN_RBRACE);
  case ';':
    return consume(TKN_SEMICOLON);
  case '.':
    return consume(TKN_PERIOD);
  case ',':
    return consume(TKN_COMMA);
  case '+':
    return consume(TKN_PLUS);
  case '-':
    return consume(TKN_MINUS);
  case '*':
    return consume(TKN_STAR);
  case '%':
    return consume(TKN_PERCENT);
  case '@':
    return consume(TKN_AT);
  }
}

Token Lexer::consume(TknTy ty) {
  Token tkn = Token(ty, curr_pos_, curr_line_);
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
  if (curr_line_ == buffer_.size() - 1 || on_new_line) {
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

bool Lexer::finished() { return curr_line_ >= buffer_.size(); }

void Lexer::skip_whitespace() {
  while (std::isspace(curr_char_.value())) {
    advance();
  }
}
