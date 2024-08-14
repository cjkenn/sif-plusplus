#pragma once

#include "sif/Parser/token.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
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
  ProgramAST(std::vector<std::unique_ptr<ASTNode>> blocks) {
    kind_ = ASTKind::Program;
    blocks_ = std::move(blocks);
  }

  ~ProgramAST() {}

private:
  std::vector<std::unique_ptr<ASTNode>> blocks_;
};

class BlockAST : public ASTNode {
public:
  BlockAST(std::vector<std::unique_ptr<ASTNode>> decls, size_t scope) {
    kind_ = ASTKind::Block;
    decls_ = std::move(decls);
    scope_ = scope;
  }

  ~BlockAST() {}

private:
  std::vector<std::unique_ptr<ASTNode>> decls_;
  size_t scope_;
};

class IfStmtAST : public ASTNode {
public:
  IfStmtAST(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> ifs,
            std::vector<std::shared_ptr<ASTNode>> elifs,
            std::vector<std::shared_ptr<ASTNode>> elses) {
    kind_ = ASTKind::IfStmt;
    cond_expr = std::move(cond);
    if_stmts = std::move(ifs);
    elif_exprs = std::move(elifs);
    else_stmts = std::move(elses);
  }

  ~IfStmtAST() {}

  std::shared_ptr<ASTNode> cond_expr;
  std::shared_ptr<ASTNode> if_stmts;
  std::vector<std::shared_ptr<ASTNode>> elif_exprs;
  std::vector<std::shared_ptr<ASTNode>> else_stmts;
};

class ElifStmtAST : public ASTNode {
public:
  ElifStmtAST(std::unique_ptr<ASTNode> cond_expr,
              std::unique_ptr<ASTNode> stmts) {
    kind_ = ASTKind::ElifStmt;
    cond_expr_ = std::move(cond_expr);
    stmts_ = std::move(stmts);
  }

  ~ElifStmtAST() {}

  std::unique_ptr<ASTNode> cond_expr_;
  std::unique_ptr<ASTNode> stmts_;
};

class ForStmtAST : public ASTNode {
public:
  ForStmtAST(std::unique_ptr<ASTNode> var_list,
             std::unique_ptr<ASTNode> in_expr_list,
             std::unique_ptr<ASTNode> stmts) {
    kind_ = ASTKind::ForStmt;
    var_list_ = std::move(var_list);
    in_expr_list_ = std::move(in_expr_list);
    stmts_ = std::move(stmts);
  }

  ~ForStmtAST() {}

  std::unique_ptr<ASTNode> var_list_;
  std::unique_ptr<ASTNode> in_expr_list_;
  std::unique_ptr<ASTNode> stmts_;
};

class ReturnStmtAST : public ASTNode {
public:
  ReturnStmtAST(std::optional<std::unique_ptr<ASTNode>> ret_expr) {
    kind_ = ASTKind::ReturnStmt;
    ret_expr_ = std::move(ret_expr);
  }

  ~ReturnStmtAST() {}

  std::optional<std::unique_ptr<ASTNode>> ret_expr_;
};

class ExprStmtAST : public ASTNode {
public:
  ExprStmtAST(std::unique_ptr<ASTNode> expr) {
    kind_ = ASTKind::ExprStmt;
    expr_ = std::move(expr);
  }

  ~ExprStmtAST() {}

  std::unique_ptr<ASTNode> expr_;
};

class VarDeclAST : public ASTNode {
public:
  VarDeclAST(std::unique_ptr<Token> ident_token, bool is_global,
             std::optional<std::unique_ptr<ASTNode>> rhs) {
    ident_token_ = std::move(ident_token);
    is_global_ = is_global;
    rhs_ = std::move(rhs);
  }

  ~VarDeclAST() {}

  std::unique_ptr<Token> ident_token_;
  bool is_global_;
  std::optional<std::unique_ptr<ASTNode>> rhs_;
};

class FnDeclAST : public ASTNode {
  FnDeclAST(std::unique_ptr<Token> ident_token, std::unique_ptr<ASTNode> params,
            std::unique_ptr<ASTNode> body, size_t scope) {
    ident_token_ = std::move(ident_token);
    params_ = std::move(params);
    body_ = std::move(body);
    scope_ = scope;
  }

  ~FnDeclAST() {}

  std::unique_ptr<Token> ident_token_;
  std::unique_ptr<ASTNode> params_;
  std::unique_ptr<ASTNode> body_;
  size_t scope_;
};

} // namespace sif
