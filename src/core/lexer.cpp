// src/core/lexer.cpp
#include "lexer.h"
#include <cctype>

namespace nova {

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},
    {"const", TokenType::CONST},
    {"auto", TokenType::AUTO},
    {"type", TokenType::TYPE},
    {"struct", TokenType::STRUCT},
    {"impl", TokenType::IMPL},
    {"trait", TokenType::TRAIT},
    {"class", TokenType::CLASS},
    {"if", TokenType::IF},
    {"elif", TokenType::ELIF},
    {"else", TokenType::ELSE},
    {"match", TokenType::MATCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"do", TokenType::DO},
    {"foreach", TokenType::FOREACH},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"pass", TokenType::PASS},
    {"func", TokenType::FUNC},
    {"return", TokenType::RETURN},
    {"async", TokenType::ASYNC},
    {"await", TokenType::AWAIT},
    {"generator", TokenType::GENERATOR},
    {"yield", TokenType::YIELD},
    {"import", TokenType::IMPORT},
    {"module", TokenType::MODULE},
    {"export", TokenType::EXPORT},
    {"static", TokenType::STATIC},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"null", TokenType::NULL_T},
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT},
    {"double", TokenType::DOUBLE},
    {"string", TokenType::STRING},
    {"char", TokenType::CHAR},
    {"bool", TokenType::BOOL},
    {"byte", TokenType::BYTE},
    {"void", TokenType::VOID},
    {"any", TokenType::ANY},
};

Lexer::Lexer(const std::string& source) 
    : source(source), start(0), current(0), line(1), column(1) {}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::addToken(TokenType type) {
    std::string lexeme = source.substr(start, current - start);
    tokens.push_back(Token(type, lexeme, line, column - (current - start)));
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back(Token(type, literal, line, column - (current - start)));
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::readIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    addToken(type);
}

void Lexer::readNumber() {
    while (std::isdigit(peek())) advance();
    
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) advance();
        addToken(TokenType::FLOAT_LITERAL);
    } else {
        addToken(TokenType::INT_LITERAL);
    }
}

void Lexer::readString() {
    advance(); // Skip opening "
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\\') advance();
        advance();
    }
    if (isAtEnd()) {
        addToken(TokenType::ERROR, "Unterminated string");
        return;
    }
    advance(); // Skip closing "
    
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void Lexer::readChar() {
    advance(); // Skip opening '
    if (isAtEnd()) {
        addToken(TokenType::ERROR, "Unterminated character");
        return;
    }
    
    char c = advance();
    
    if (peek() != '\'') {
        addToken(TokenType::ERROR, "Invalid character literal");
        return;
    }
    advance(); // Skip closing '
    
    addToken(TokenType::CHAR_LITERAL, std::string(1, c));
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    
    while (!isAtEnd()) {
        start = current;
        char c = advance();
        
        switch (c) {
            case '(': addToken(TokenType::LPAREN); break;
            case ')': addToken(TokenType::RPAREN); break;
            case '{': addToken(TokenType::LBRACE); break;
            case '}': addToken(TokenType::RBRACE); break;
            case '[': addToken(TokenType::LBRACK); break;
            case ']': addToken(TokenType::RBRACK); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case ':': addToken(TokenType::COLON); break;
            case '+': 
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::PLUS_ASSIGN);
                } else {
                    addToken(TokenType::PLUS);
                }
                break;
            case '-':
                if (peek() == '>') {
                    advance();
                    addToken(TokenType::ARROW);
                } else if (peek() == '=') {
                    advance();
                    addToken(TokenType::MINUS_ASSIGN);
                } else {
                    addToken(TokenType::MINUS);
                }
                break;
            case '*': addToken(TokenType::STAR); break;
            case '/':
                if (peek() == '/') {
                    skipComment();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case '%': addToken(TokenType::PERCENT); break;
            case '=':
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::EQ);
                } else {
                    addToken(TokenType::ASSIGN);
                }
                break;
            case '!':
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::NE);
                } else {
                    addToken(TokenType::NOT);
                }
                break;
            case '<':
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::LE);
                } else {
                    addToken(TokenType::LT);
                }
                break;
            case '>':
                if (peek() == '=') {
                    advance();
                    addToken(TokenType::GE);
                } else {
                    addToken(TokenType::GT);
                }
                break;
            case '&':
                if (peek() == '&') {
                    advance();
                    addToken(TokenType::AND);
                }
                break;
            case '|':
                if (peek() == '|') {
                    advance();
                    addToken(TokenType::OR);
                } else if (peek() == '>') {
                    advance();
                    addToken(TokenType::PIPE_ARROW);
                }
                break;
            case '~':
                if (peek() == '>') {
                    advance();
                    addToken(TokenType::TILDE_ARROW);
                }
                break;
            case '"':
                readString();
                break;
            case '\'':
                readChar();
                break;
            default:
                if (std::isspace(c)) {
                    skipWhitespace();
                    continue;
                } else if (std::isalpha(c) || c == '_') {
                    readIdentifier();
                } else if (std::isdigit(c)) {
                    readNumber();
                } else {
                    addToken(TokenType::ERROR, "Unexpected character");
                }
                break;
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_T, "", line, column));
    return tokens;
}

} // namespace nova