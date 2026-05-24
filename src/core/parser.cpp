// src/core/parser.cpp
#include "parser.h"
#include <iostream>

namespace nova {

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), current(0) {}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_T;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match({TokenType::ASSIGN})) {
        auto value = assignment();
        
        if (expr->type == ExprType::VARIABLE) {
            auto* var = static_cast<VariableExpr*>(expr.get());
            auto assign = std::make_unique<AssignExpr>();
            assign->name = var->name;
            assign->value = std::move(value);
            return assign;
        }
        
        throw std::runtime_error("Invalid assignment target");
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({TokenType::OR})) {
        Token op = previous();
        auto right = logicalAnd();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match({TokenType::AND})) {
        Token op = previous();
        auto right = equality();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::EQ, TokenType::NE})) {
        Token op = previous();
        auto right = comparison();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = addition();
    
    while (match({TokenType::LT, TokenType::LE, TokenType::GT, TokenType::GE})) {
        Token op = previous();
        auto right = addition();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::addition() {
    auto expr = multiplication();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = multiplication();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::multiplication() {
    auto expr = unary();
    
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        Token op = previous();
        auto right = unary();
        auto binary = std::make_unique<BinaryExpr>();
        binary->left = std::move(expr);
        binary->op = op.type;
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token op = previous();
        auto operand = unary();
        auto unaryExpr = std::make_unique<UnaryExpr>();
        unaryExpr->op = op.type;
        unaryExpr->operand = std::move(operand);
        return unaryExpr;
    }
    
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    auto expr = primary();
    
    while (match({TokenType::LPAREN})) {
        std::vector<std::unique_ptr<Expr>> arguments;
        
        if (!check(TokenType::RPAREN)) {
            do {
                arguments.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')' after arguments");
        
        auto callExpr = std::make_unique<CallExpr>();
        callExpr->callee = std::move(expr);
        callExpr->arguments = std::move(arguments);
        expr = std::move(callExpr);
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::INT_LITERAL})) {
        auto literal = std::make_unique<LiteralExpr>();
        int64_t val = std::stoll(previous().lexeme);
        literal->value = val;
        return literal;
    }
    
    if (match({TokenType::FLOAT_LITERAL})) {
        auto literal = std::make_unique<LiteralExpr>();
        double val = std::stod(previous().lexeme);
        literal->value = val;
        return literal;
    }
    
    if (match({TokenType::STRING_LITERAL})) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->value = previous().lexeme;
        return literal;
    }
    
    if (match({TokenType::TRUE})) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->value = true;
        return literal;
    }
    
    if (match({TokenType::FALSE})) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->value = false;
        return literal;
    }
    
    if (match({TokenType::NULL_T})) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->value = std::monostate();
        return literal;
    }
    
    if (match({TokenType::IDENTIFIER})) {
        auto var = std::make_unique<VariableExpr>();
        var->name = previous().lexeme;
        return var;
    }
    
    if (match({TokenType::LPAREN})) {
        auto expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw std::runtime_error("Expected expression");
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET, TokenType::CONST})) return variableDecl();
    if (match({TokenType::STRUCT})) return structDecl();
    if (match({TokenType::FUNC})) return functionDecl();
    return statement();
}

std::unique_ptr<Stmt> Parser::variableDecl() {
    TokenType varType = previous().type;
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    std::string typeAnnotation;
    if (match({TokenType::COLON})) {
        typeAnnotation = consume(TokenType::IDENTIFIER, "Expected type name").lexeme;
    }
    
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    auto decl = std::make_unique<VariableDeclStmt>();
    decl->name = name.lexeme;
    decl->typeAnnotation = typeAnnotation;
    decl->initializer = std::move(initializer);
    decl->isNullable = (varType == TokenType::LET && typeAnnotation.empty());
    
    return decl;
}

std::unique_ptr<Stmt> Parser::functionDecl() {
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    std::vector<std::pair<std::string, std::string>> params;
    if (!check(TokenType::RPAREN)) {
        do {
            Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
            std::string paramType;
            if (match({TokenType::COLON})) {
                paramType = consume(TokenType::IDENTIFIER, "Expected parameter type").lexeme;
            }
            params.push_back({paramName.lexeme, paramType});
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    std::string returnType;
    if (match({TokenType::ARROW})) {
        returnType = consume(TokenType::IDENTIFIER, "Expected return type").lexeme;
    }
    
    auto body = blockStatement();
    
    auto func = std::make_unique<FuncDeclStmt>();
    func->name = name.lexeme;
    func->params = std::move(params);
    func->returnType = returnType;
    func->isAsync = false;
    func->isStatic = false;
    
    if (body->type == StmtType::BLOCK) {
        auto* block = static_cast<BlockStmt*>(body.get());
        func->body = std::move(block->statements);
    }
    
    return func;
}

std::unique_ptr<Stmt> Parser::structDecl() {
    Token name = consume(TokenType::IDENTIFIER, "Expected struct name");
    consume(TokenType::LBRACE, "Expected '{' after struct name");
    
    std::vector<std::pair<std::string, std::string>> fields;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        std::string typeName = consume(TokenType::IDENTIFIER, "Expected field type").lexeme;
        Token fieldName = consume(TokenType::IDENTIFIER, "Expected field name");
        fields.push_back({fieldName.lexeme, typeName});
        consume(TokenType::SEMICOLON, "Expected ';' after field");
    }
    
    consume(TokenType::RBRACE, "Expected '}' after struct fields");
    
    auto structDecl = std::make_unique<StructDeclStmt>();
    structDecl->name = name.lexeme;
    structDecl->fields = std::move(fields);
    
    return structDecl;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::BREAK, TokenType::CONTINUE})) {
        consume(TokenType::SEMICOLON, "Expected ';' after control statement");
        auto stmt = std::make_unique<ExpressionStmt>();
        return stmt;
    }
    if (match({TokenType::LBRACE})) return blockStatement();
    
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LPAREN, "Expected '(' after if");
    auto condition = expression();
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }
    
    auto ifStmt = std::make_unique<IfStmt>();
    ifStmt->condition = std::move(condition);
    ifStmt->thenBranch = std::move(thenBranch);
    ifStmt->elseBranch = std::move(elseBranch);
    
    return ifStmt;
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LPAREN, "Expected '(' after while");
    auto condition = expression();
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    
    auto body = statement();
    
    auto whileStmt = std::make_unique<WhileStmt>();
    whileStmt->condition = std::move(condition);
    whileStmt->body = std::move(body);
    
    return whileStmt;
}

std::unique_ptr<Stmt> Parser::blockStatement() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume(TokenType::RBRACE, "Expected '}' after block");
    
    auto block = std::make_unique<BlockStmt>();
    block->statements = std::move(statements);
    
    return block;
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after return");
    
    auto ret = std::make_unique<ReturnStmt>();
    ret->value = std::move(value);
    return ret;
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    
    auto stmt = std::make_unique<ExpressionStmt>();
    stmt->expr = std::move(expr);
    return stmt;
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return statements;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        TokenType type = peek().type;
        if (type == TokenType::STRUCT ||
            type == TokenType::FUNC ||
            type == TokenType::LET ||
            type == TokenType::CONST ||
            type == TokenType::IF ||
            type == TokenType::WHILE ||
            type == TokenType::RETURN) {
            return;
        }
        advance();
    }
}

} // namespace nova