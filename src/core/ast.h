// src/core/ast.h
#pragma once
#include "token.h"
#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace nova {

// Forward declarations
struct Expr;
struct Stmt;

// Expression types enum
enum class ExprType {
    BINARY, UNARY, LITERAL, VARIABLE, ASSIGN, CALL
};

// Statement types enum
enum class StmtType {
    EXPRESSION, VARIABLE_DECL, BLOCK, IF, WHILE, RETURN,
    FUNC_DECL, STRUCT_DECL, IMPL_BLOCK
};

// Base Expr
struct Expr {
    ExprType type;
    virtual ~Expr() = default;
};

// Binary expression
struct BinaryExpr : Expr {
    BinaryExpr() { type = ExprType::BINARY; }
    std::unique_ptr<Expr> left;
    TokenType op;
    std::unique_ptr<Expr> right;
};

// Unary expression
struct UnaryExpr : Expr {
    UnaryExpr() { type = ExprType::UNARY; }
    TokenType op;
    std::unique_ptr<Expr> operand;
};

// Literal expression
struct LiteralExpr : Expr {
    LiteralExpr() { type = ExprType::LITERAL; }
    std::variant<std::monostate, int64_t, double, std::string, char, bool> value;
};

// Variable expression
struct VariableExpr : Expr {
    VariableExpr() { type = ExprType::VARIABLE; }
    std::string name;
};

// Assign expression
struct AssignExpr : Expr {
    AssignExpr() { type = ExprType::ASSIGN; }
    std::string name;
    std::unique_ptr<Expr> value;
};

// Call expression
struct CallExpr : Expr {
    CallExpr() { type = ExprType::CALL; }
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
};

// Base Stmt
struct Stmt {
    StmtType type;
    virtual ~Stmt() = default;
};

// Expression statement
struct ExpressionStmt : Stmt {
    ExpressionStmt() { type = StmtType::EXPRESSION; }
    std::unique_ptr<Expr> expr;
};

// Variable declaration
struct VariableDeclStmt : Stmt {
    VariableDeclStmt() { type = StmtType::VARIABLE_DECL; }
    std::string name;
    std::string typeAnnotation;
    std::unique_ptr<Expr> initializer;
    bool isNullable;
};

// Block statement
struct BlockStmt : Stmt {
    BlockStmt() { type = StmtType::BLOCK; }
    std::vector<std::unique_ptr<Stmt>> statements;
};

// If statement
struct IfStmt : Stmt {
    IfStmt() { type = StmtType::IF; }
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
};

// While statement
struct WhileStmt : Stmt {
    WhileStmt() { type = StmtType::WHILE; }
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

// Return statement
struct ReturnStmt : Stmt {
    ReturnStmt() { type = StmtType::RETURN; }
    std::unique_ptr<Expr> value;
};

// Function declaration
struct FuncDeclStmt : Stmt {
    FuncDeclStmt() { type = StmtType::FUNC_DECL; }
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::string returnType;
    std::vector<std::unique_ptr<Stmt>> body;
    bool isAsync;
    bool isStatic;
};

// Struct declaration
struct StructDeclStmt : Stmt {
    StructDeclStmt() { type = StmtType::STRUCT_DECL; }
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
};

// Impl block
struct ImplBlockStmt : Stmt {
    ImplBlockStmt() { type = StmtType::IMPL_BLOCK; }
    std::string typeName;
    std::vector<std::unique_ptr<Stmt>> methods;
};

} // namespace nova