#include "Parser.h"

#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}
const Token& Parser::peek() const {
    return tokens[current];
}
const Token& Parser::previous() const {
    return tokens[current - 1];
}
const Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}
bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}
const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}
std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        if (match(TokenType::Fn)) {
            program->functions.push_back(functionDeclaration());
        } else {
            throw std::runtime_error(
                "Expected function declaration"
            );
        }
    }
    return program;
}
std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration()
{
    const Token& name = consume(
        TokenType::Identifier,
        "Expected function name after 'fn'."
    );

    consume(
        TokenType::LeftParen,
        "Expected '(' after function name."
    );

    consume(
        TokenType::RightParen,
        "Expected ')' after function parameters."
    );

    consume(
        TokenType::LeftBrace,
        "Expected '{' before function body."
    );

    auto function =
        std::make_unique<FunctionDeclaration>(name.lexeme);

    while (!check(TokenType::RightBrace) && !isAtEnd())
    {
        function->body.push_back(statement());
    }

    consume(
        TokenType::RightBrace,
        "Expected '}' after function body."
    );

    return function;
}
std::unique_ptr<Statement> Parser::statement() {
    return functionCall();
}
std::unique_ptr<Statement> Parser::functionCall() {
    const Token& name = consume(
        TokenType::Identifier,
        "Expected function name"
    );
    consume(
        TokenType::LeftParen,
        "Expected '(' after function name"
    );
    auto call = std::make_unique<FunctionCall>(name.lexeme);

    if (!check(TokenType::RightParen)) {
        call->arguments.push_back(expression());
    }
    consume(
        TokenType::RightParen,
        "Expected ')' after function arguments"
    );
    consume(
        TokenType::Semicolon,
        "Expected ';' after function call"
    );
    return call;
}
std::unique_ptr<Expression> Parser::expression() {
    return primary();
}
std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::String)) {
        return std::make_unique<StringLiteral>(
            previous().lexeme
        );
    }
    throw std::runtime_error(
        "Expected expression."
    );
}