// src/core/lexer.h
#pragma once
#include "token.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace nova {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t start;
    size_t current;
    int line;
    int column;
    std::vector<Token> tokens;
    
    char advance();
    char peek();
    char peekNext();
    bool isAtEnd();
    void skipWhitespace();
    void skipComment();
    
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    
    void readIdentifier();
    void readNumber();
    void readString();
    void readChar();
    
    static std::unordered_map<std::string, TokenType> keywords;
};

} // namespace nova