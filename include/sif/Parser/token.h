#pragma once

namespace sif {
enum TknTy {
  TKN_LPAREN,
  TKN_RPAREN,
  TKN_LBRACE,
  TKN_RBRACE,
  TKN_LBRACK,
  TKN_RBRACK

};

class Token {
public:
  Token(){};
  ~Token(){};

  auto GetTy() const { return ty_; }
  auto GetLen() const { return len_; }
  auto GetPos() const { return pos_; }
  auto GetLine() const { return line_; }

private:
  TknTy ty_;
  int len_;
  int pos_;
  int line_;
};
} // namespace sif
