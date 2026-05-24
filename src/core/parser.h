// src/core/parser.h
#pragma once
#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nova {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
    
private:
    std::vector<Token> tokens;
    size_t current;
    
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    // Parsing methods
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> addition();
    std::unique_ptr<Expr> multiplication();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();
    
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> variableDecl();
    std::unique_ptr<Stmt> functionDecl();
    std::unique_ptr<Stmt> structDecl();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> blockStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> expressionStatement();
};

} // namespace nova