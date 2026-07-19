#include "../include/Lexer.h"
#include <iostream>
Lexer::Lexer(const std::string& source)
{
    this->source = source;
}
std::vector<Token> Lexer::tokenize()
{
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({
        TokenType::EndOfFile,
        "",
        line,
        column
    });
    return tokens;
}
void Lexer::scanToken()
{
    char c = advance();
    switch(c) {
        case '(':
            addToken(TokenType::LeftParen);
            break;
             case ')':
            addToken(TokenType::RightParen);
            break;
        case '{':
            addToken(TokenType::LeftBrace);
            break;

        case '}':
            addToken(TokenType::RightBrace);
            break;

        case ';':
            addToken(TokenType::Semicolon);
            break;

        case ',':
            addToken(TokenType::Comma);
            break;


        case '+':
            addToken(TokenType::Plus);
            break;


        case '-':
            addToken(TokenType::Minus);
            break;


        case '*':
            addToken(TokenType::Star);
            break;


        case '/':
            addToken(TokenType::Slash);
            break;

        case '"':
            string();
            break;


        default:

            if (isAlpha(c))
            {
                identifier();
            }
            else if (isDigit(c))
            {
                number();
            }

            break;
    }
}
char Lexer::advance()
{
    char c = source[current];
    current++;
    column++;
    return c;
}
char Lexer::peek() {
    if (isAtEnd())
        return '\0';
    return source[current];
}
char Lexer::peekNext() {
    if (current +1>=source.length())
        return '\0';
    return source[current + 1];
}
bool Lexer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (source[current]!=expected)
        return false;
    current++;
    column++;
    return true;
}
bool Lexer::isAtEnd()
{
    return current >= source.length();
}
void Lexer::addToken(TokenType type) {
    tokens.push_back({
        type,
        source.substr(start, current-start),
        line,
        column,
    });
}
void Lexer::addToken(TokenType type, const std::string& text) {
    tokens.push_back({
        type,
        text,
        line,
        column
    });
}
void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                line++;
                column = 1;
                break;
            default:
                return;
        }
    }
}
bool Lexer::isDigit(char c) {
    return c >= '0' && c <='9';
}
bool Lexer::isAlpha(char c) {
    return (c>='a'&&c<='z') || (c>='A'&&c<='Z')||c=='_';
}
bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
void Lexer::identifier() {
    while(isAlphaNumeric(peek())) {
        advance();
    }
    std::string text = source.substr(
        start,
        current-start
    );
    TokenType type = keywordType(text);
    addToken(type);
}
void Lexer::number() {

}
void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    if (isAtEnd()) {
        return;
    }
    advance();
    std::string value = source.substr(start + 1, current - start -2);
    addToken(TokenType::String, value);
}
TokenType Lexer::keywordType(const std::string& text) {
    if (text == "fn")
        return TokenType::Fn;
    if (text=="class")
        return TokenType::Class;
    if (text=="return")
        return TokenType::Return;
    if (text=="if")
        return TokenType::If;
    if (text=="else")
        return TokenType::Else;
    if (text=="while")
        return TokenType::While;
    if (text=="for")
        return TokenType::For;
    if (text=="true")
        return TokenType::True;
    if (text=="false")
        return TokenType::False;
    if (text=="null")
        return TokenType::Null;
    return TokenType::Identifier;
}