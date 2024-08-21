#pragma once

#include "sif/Parser/token.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
class ASTNode;
typedef std::unique_ptr<ASTNode> ASTPtr;

enum class ASTKind {
  Program,
  Block,
  IfStmt,
  ElifStmt,
  ForStmt,
  ReturnStmt,
  ExprStmt,
  VarDecl,
  FnDecl,
  FnParams,
  IdentPair,
  ItemList,
  TableItem,
  Table,
  TableAccess,
  Array,
  ArrayItems,
  ArrayAccess,
  ArrayMutExpr,
  FnCallExpr,
  VarAssignExpr,
  BinaryExpr,
  UnaryExpr,
  PrimaryExpr,
  Empty
};

class ASTNode {
public:
  virtual ~ASTNode() = default;

  ASTKind GetKind() const { return kind_; }

protected:
  ASTKind kind_;
};

class ProgramAST : public ASTNode {
public:
  ProgramAST(std::vector<ASTPtr> blocks) {
    kind_ = ASTKind::Program;
    blocks_ = std::move(blocks);
  }

  ~ProgramAST() {}

private:
  std::vector<ASTPtr> blocks_;
};

class BlockAST : public ASTNode {
public:
  BlockAST(std::vector<ASTPtr> decls, size_t scope) {
    kind_ = ASTKind::Block;
    decls_ = std::move(decls);
    scope_ = scope;
  }

  ~BlockAST() {}

private:
  std::vector<ASTPtr> decls_;
  size_t scope_;
};

class IfStmtAST : public ASTNode {
public:
  IfStmtAST(ASTPtr cond, ASTPtr ifs, std::vector<ASTPtr> elifs,
            std::vector<ASTPtr> elses) {
    kind_ = ASTKind::IfStmt;
    cond_expr = std::move(cond);
    if_stmts = std::move(ifs);
    elif_exprs = std::move(elifs);
    else_stmts = std::move(elses);
  }

  ~IfStmtAST() {}

  ASTPtr cond_expr;
  ASTPtr if_stmts;
  std::vector<ASTPtr> elif_exprs;
  std::vector<ASTPtr> else_stmts;
};

class ElifStmtAST : public ASTNode {
public:
  ElifStmtAST(ASTPtr cond_expr, ASTPtr stmts) {
    kind_ = ASTKind::ElifStmt;
    cond_expr_ = std::move(cond_expr);
    stmts_ = std::move(stmts);
  }

  ~ElifStmtAST() {}

  ASTPtr cond_expr_;
  ASTPtr stmts_;
};

class ForStmtAST : public ASTNode {
public:
  ForStmtAST(ASTPtr var_list, ASTPtr in_expr_list, ASTPtr stmts) {
    kind_ = ASTKind::ForStmt;
    var_list_ = std::move(var_list);
    in_expr_list_ = std::move(in_expr_list);
    stmts_ = std::move(stmts);
  }

  ~ForStmtAST() {}

  ASTPtr var_list_;
  ASTPtr in_expr_list_;
  ASTPtr stmts_;
};

class ReturnStmtAST : public ASTNode {
public:
  ReturnStmtAST(std::optional<ASTPtr> ret_expr) {
    kind_ = ASTKind::ReturnStmt;
    ret_expr_ = std::move(ret_expr);
  }

  ~ReturnStmtAST() {}

  std::optional<ASTPtr> ret_expr_;
};

class ExprStmtAST : public ASTNode {
public:
  ExprStmtAST(ASTPtr expr) {
    kind_ = ASTKind::ExprStmt;
    expr_ = std::move(expr);
  }

  ~ExprStmtAST() {}

  ASTPtr expr_;
};

class VarDeclAST : public ASTNode {
public:
  VarDeclAST(std::unique_ptr<Token> ident_token, bool is_global,
             std::optional<ASTPtr> rhs) {
    ident_token_ = std::move(ident_token);
    is_global_ = is_global;
    rhs_ = std::move(rhs);
  }

  ~VarDeclAST() {}

  std::unique_ptr<Token> ident_token_;
  bool is_global_;
  std::optional<ASTPtr> rhs_;
};

class FnDeclAST : public ASTNode {
  FnDeclAST(std::unique_ptr<Token> ident_token, ASTPtr params, ASTPtr body,
            size_t scope) {
    ident_token_ = std::move(ident_token);
    params_ = std::move(params);
    body_ = std::move(body);
    scope_ = scope;
  }

  ~FnDeclAST() {}

  std::unique_ptr<Token> ident_token_;
  ASTPtr params_;
  ASTPtr body_;
  size_t scope_;
};

class PrimaryExprAST : public ASTNode {
public:
  PrimaryExprAST(Token tkn) : token_(TokenKind::Eof, 0, 0) {
    token_ = tkn;
    kind_ = ASTKind::PrimaryExpr;
  }

  Token token_;
};

class VarAssignAST : public ASTNode {
public:
  VarAssignAST(Token ident_tkn, bool is_global, ASTPtr rhs)
      : ident_tkn_(TokenKind::Eof, 0, 0) {
    ident_tkn_ = ident_tkn;
    is_global_ = is_global;
    rhs_ = std::move(rhs);
  }

  Token ident_tkn_;
  bool is_global_;
  ASTPtr rhs_;
};

class ArrayAccessAST : public ASTNode {
public:
  ArrayAccessAST(Token array_tkn, ASTPtr index)
      : array_tkn_(TokenKind::Eof, 0, 0) {
    array_tkn_ = array_tkn;
    index_ = std::move(index);
  }

  Token array_tkn_;
  ASTPtr index_;
};

class ArrayMutExpr : public ASTNode {
public:
  ArrayMutExpr(Token array_tkn, ASTPtr index, ASTPtr rhs)
      : array_tkn_(TokenKind::Eof, 0, 0) {
    array_tkn_ = array_tkn;
    index_ = std::move(index);
    rhs_ = std::move(rhs);
  }

  Token array_tkn_;
  ASTPtr index_;
  ASTPtr rhs_;
};

} // namespace sif
