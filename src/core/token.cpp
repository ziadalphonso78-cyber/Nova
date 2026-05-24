// src/core/token.cpp
#include "token.h"

namespace nova {

Token::Token(TokenType type, const std::string& lexeme, int line, int col)
    : type(type), lexeme(lexeme), line(line), column(col) {}

std::string Token::toString() const {
    return "Token(" + std::to_string(static_cast<int>(type)) + ", " + lexeme + ")";
}

} // namespace nova