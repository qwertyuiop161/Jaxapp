#pragma once

#include <string>
#include <vector>
#include "Token.h"
class Lexer {

public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
private:
    std::string source;
    std::vector<Token> tokens;
    size_t current = 0;
    size_t start = 0;
    int line = 1;
    int column  = 1;
    
    void scanToken();
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& text);
    void identifier();
    void number();
    void string();
    void skipWhitespace();
    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    TokenType keywordType(const std::string& text);
};
