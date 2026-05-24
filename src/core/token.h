// src/core/token.h
#pragma once
#include <string>
#include <variant>
#include <unordered_map>
#include <iostream>

namespace nova {

enum class TokenType {
    // Keywords
    LET, CONST, AUTO, TYPE, STRUCT, IMPL, TRAIT, CLASS,
    IF, ELIF, ELSE, MATCH, CASE, DEFAULT,
    FOR, WHILE, DO, FOREACH, BREAK, CONTINUE, PASS,
    FUNC, RETURN, ASYNC, AWAIT, GENERATOR, YIELD,
    IMPORT, MODULE, EXPORT, STATIC, PUBLIC, PRIVATE,
    
    // Types
    INT, FLOAT, DOUBLE, STRING, CHAR, BOOL, BYTE, VOID, ANY, NULL_T,
    UINT, INT8, INT16, INT32, INT64,
    
    // Literals
    TRUE, FALSE,
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, NE, LT, LE, GT, GE,
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN,
    AND, OR, NOT,
    ARROW, PIPE_ARROW, TILDE_ARROW,
    DOT, COMMA, COLON, SEMICOLON,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK,
    
    // Literals
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL, CHAR_LITERAL,
    
    // Special
    EOF_T, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token() : type(TokenType::EOF_T), line(0), column(0) {}
    Token(TokenType type, const std::string& lexeme, int line, int col);
    
    std::string toString() const;
};

} // namespace nova