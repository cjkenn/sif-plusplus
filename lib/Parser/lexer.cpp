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
  if (isdigit(curr)) {
    return lex_num_lit();
  } else if (isalpha(curr)) {
    return lex_ident();
  }

  switch (curr) {
  case '(':
    return consume(TKN_LPAREN);
  case ')':
    return consume(TKN_RPAREN);
  case '{':
    return consume(TKN_LBRACE);
  case '}':
    return consume(TKN_RBRACE);
  case '[': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '[') {
      Token result = consume(TKN_DOUBLELBRACK);
      advance();
      return result;
    } else {
      return consume(TKN_LBRACK);
    }
  }
  case ']': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == ']') {
      Token result = consume(TKN_DOUBLERBRACK);
      advance();
      return result;
    } else {
      return consume(TKN_RBRACK);
    }
  }
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
  case '/': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '/') {
      while (curr_char_.value() != '\n') {
        advance();
      }
      return Lex();
    } else {
      return consume(TKN_SLASH);
    }
  }
  case '=': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TKN_EQEQ);
      advance();
      return result;
    } else if (next.has_value() && next.value() == '>') {
      Token result = consume(TKN_EQARROW);
      advance();
      return result;
    } else {
      return consume(TKN_RBRACK);
    }
  }
  case '<': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TKN_LTEQ);
      advance();
      return result;
    } else {
      return consume(TKN_LT);
    }
  }
  case '>': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TKN_GTEQ);
      advance();
      return result;
    } else {
      return consume(TKN_GT);
    }
  }
  case '!': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '=') {
      Token result = consume(TKN_BANGEQ);
      advance();
      return result;
    } else {
      return consume(TKN_BANG);
    }
  }
  case '&': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '&') {
      Token result = consume(TKN_AMPAMP);
      advance();
      return result;
    } else {
      return consume(TKN_AMP);
    }
  }
  case '|': {
    std::optional<char> next = peek();
    if (next.has_value() && next.value() == '|') {
      Token result = consume(TKN_PIPEPIPE);
      advance();
      return result;
    } else {
      return consume(TKN_PIPE);
    }
  }
  case '"':
    return lex_str_lit();
  default:
    return Token(TKN_EOF, 0, 0);
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
  return Token(TKN_EOF, 0, 0);
}

Token Lexer::lex_num_lit() {
  std::string literal;
  int num_start = curr_pos_;
  int num_line = curr_line_;

  bool dot_allowed = true;
  while (!finished() && curr_char_.has_value() &&
         (isdigit(curr_char_.value()) || curr_char_ == '.')) {
    assert((curr_char_.value() == '.' && dot_allowed) &&
           "incorrectly formatted number!");

    if (curr_char_.value() == '.') {
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

  while (!finished() && curr_char_.has_value() && isalpha(curr_char_.value())) {
    literal += curr_char_.value();
    advance();
  }

  if (reserved_words_.contains(literal)) {
    TknTy curr_type = reserved_words_[literal];
    Token next = Token(curr_type, ident_start, ident_line);
    next.SetIdentLit(literal);
    return next;
  } else {
    Token next = Token(TKN_IDENT, ident_start, ident_line);
    next.SetIdentLit(literal);
    return next;
  }
}

Token Lexer::consume_str_lit(std::string str, int pos, int line) {
  Token tkn = Token(TKN_STRLIT, pos, line);
  tkn.SetStrLit(str);
  advance();
  return tkn;
}

Token Lexer::consume_num_lit(std::string num, int pos, int line) {
  Token tkn = Token(TKN_NUMLIT, pos, line);
  tkn.SetNumLit(num);
  advance();
  return tkn;
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
