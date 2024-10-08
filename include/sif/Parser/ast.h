#pragma once

#include "sif/Parser/token.h"
#include <memory>
#include <optional>
#include <vector>

namespace sif {
class ASTNode;
typedef std::unique_ptr<ASTNode> ASTPtr;
typedef std::shared_ptr<ASTNode> SharedASTPtr;

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
  LiteralExpr,
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
public:
  FnDeclAST(std::unique_ptr<Token> ident_token, ASTPtr params, ASTPtr body,
            size_t scope) {
    kind_ = ASTKind::FnDecl;
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

class FnCallExprAST : public ASTNode {
public:
  FnCallExprAST(Token fn_ident_tkn, std::vector<ASTPtr> fn_params, bool is_std)
      : fn_ident_tkn_(TokenKind::Eof, 0, 0) {
    kind_ = ASTKind::FnCallExpr;
    fn_ident_tkn_ = fn_ident_tkn;
    fn_params_ = std::move(fn_params);
    is_std_ = is_std;
  }

  Token fn_ident_tkn_;
  std::vector<ASTPtr> fn_params_;
  bool is_std_;
};

class ParamListAST : public ASTNode {
public:
  ParamListAST(std::vector<ASTPtr> params) { params_ = std::move(params); }

  std::vector<ASTPtr> params_;
};

class VarAssignAST : public ASTNode {
public:
  VarAssignAST(Token ident_tkn, bool is_global, ASTPtr rhs)
      : ident_tkn_(TokenKind::Eof, 0, 0) {
    ident_tkn_ = ident_tkn;
    is_global_ = is_global;
    rhs_ = std::move(rhs);
    kind_ = ASTKind::VarAssignExpr;
  }

  Token ident_tkn_;
  bool is_global_;
  ASTPtr rhs_;
};

class TableAccessAST : public ASTNode {
public:
  TableAccessAST(Token table_tkn, ASTPtr index)
      : table_tkn_(TokenKind::Eof, 0, 0) {
    kind_ = ASTKind::TableAccess;
    table_tkn_ = table_tkn;
    index_ = std::move(index);
  }

  Token table_tkn_;
  ASTPtr index_;
};

class ArrayAccessAST : public ASTNode {
public:
  ArrayAccessAST(Token array_tkn, ASTPtr index)
      : array_tkn_(TokenKind::Eof, 0, 0) {
    array_tkn_ = array_tkn;
    index_ = std::move(index);
    kind_ = ASTKind::ArrayAccess;
  }

  Token array_tkn_;
  ASTPtr index_;
};

class ArrayMutExprAST : public ASTNode {
public:
  ArrayMutExprAST(Token array_tkn, ASTPtr index, ASTPtr rhs)
      : array_tkn_(TokenKind::Eof, 0, 0) {
    array_tkn_ = array_tkn;
    index_ = std::move(index);
    rhs_ = std::move(rhs);
    kind_ = ASTKind::ArrayMutExpr;
  }

  Token array_tkn_;
  ASTPtr index_;
  ASTPtr rhs_;
};

class BinaryExprAST : public ASTNode {
public:
  BinaryExprAST(Token op_tkn, ASTPtr lhs, ASTPtr rhs)
      : op_tkn_(TokenKind::Eof, 0, 0) {
    op_tkn_ = op_tkn;
    lhs_ = std::move(lhs);
    rhs_ = std::move(rhs);
    kind_ = ASTKind::BinaryExpr;
  }

  Token op_tkn_;
  ASTPtr lhs_;
  ASTPtr rhs_;
};

class UnaryExprAST : public ASTNode {
public:
  UnaryExprAST(Token op_tkn, ASTPtr rhs) : op_tkn_(TokenKind::Eof, 0, 0) {
    op_tkn_ = op_tkn;
    rhs_ = std::move(rhs);
    kind_ = ASTKind::UnaryExpr;
  }

  Token op_tkn_;
  ASTPtr rhs_;
};

class LiteralExprAST : public ASTNode {
public:
  LiteralExprAST(Token lit_tkn) : lit_tkn_(TokenKind::Eof, 0, 0) {
    lit_tkn_ = lit_tkn;
    kind_ = ASTKind::LiteralExpr;
  }
  Token lit_tkn_;
};

class EmptyAST : public ASTNode {
public:
  EmptyAST() { kind_ = ASTKind::Empty; }
};

} // namespace sif
