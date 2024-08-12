#pragma once

#include <memory>
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
  virtual ~ASTNode() {}

  ASTKind GetKind() const { return kind_; }

protected:
  ASTKind kind_;
};

class ProgramAST : public ASTNode {
public:
  ProgramAST(std::vector<std::unique_ptr<ASTNode>> blocks) {
    kind_ = ASTKind::Program;
    blocks_ = std::make_unique
  }
  ~ProgramAST() {}

private:
  std::vector<std::unique_ptr<ASTNode>> blocks_;
};

class BlockAST : public ASTNode {
public:
  BlockAST(std::vector<std::unique_ptr<ASTNode>> decls, size_t scope) {
    kind_ = ASTKind::Block;
    decls_ = decls;
    scope_ = scope;
  }
  ~BlockAST() {}

private:
  std::vector<std::unique_ptr<ASTNode>> decls_;
  size_t scope_;
};

class IfStmtAST : public ASTNode {
public:
  IfStmtAST(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> ifs,
            std::vector<std::unique_ptr<ASTNode>> elifs,
            std::vector<std::unique_ptr<ASTNode>> elses) {
    kind_ = ASTKind::IfStmt;
    cond_expr = cond;
    if_stmts = ifs;
    elif_exprs = elifs;
    else_stmts = elses;
  }

  std::shared_ptr<ASTNode> cond_expr;
  std::shared_ptr<ASTNode> if_stmts;
  std::vector<std::unique_ptr<ASTNode>> elif_exprs;
  std::vector<std::unique_ptr<ASTNode>> else_stmts;
};

} // namespace sif
