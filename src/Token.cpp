#include "Token.h"
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Fn:
            return "Fn";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::String:
            return "String";
        case TokenType::LeftParen:
            return "LeftParen";
        case TokenType::RightParen:
            return "RightParen";
        case TokenType::LeftBrace:
            return "LeftBrace";
        case TokenType::RightBrace:
            return "RightBrace";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::EndOfFile:
            return "EOF";
        case TokenType::Equal:
            return "Equal";
        case TokenType::KeywordString:
            return "KeywordString";
        default:
            return "Unknown";
    }
}
