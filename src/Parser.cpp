#include "Parser.h"

#include <stdexcept>
#include <utility>

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
    if (!check(type)) {
        return false;
    }
    advance();
    return true;
}
const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(
        message + " at line " + std::to_string(peek().line) + ". found token: '" + peek().lexeme + "'"
    );
}
std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    while (!isAtEnd()) {
        if (match(TokenType::Fn)) {
            program->functions.push_back(functionDeclaration());
        } else {
            throw std::runtime_error("Expected function declaration at line " + std::to_string(peek().line));
        }
    }
    return program;
}
std::unique_ptr<FunctionDeclaration>
Parser::functionDeclaration() {
    const Token& name = consume(
        TokenType::Identifier,
        "Expected function name after 'fn'"
    );

    consume(
        TokenType::LeftParen,
        "Expected '(' after function name"
    );

    consume(
        TokenType::RightParen,
        "Expected ')' after function parameters"
    );

    consume(
        TokenType::LeftBrace,
        "Expected '{' before function body"
    );

    auto function =
        std::make_unique<FunctionDeclaration>(name.lexeme);

    while (
        !check(TokenType::RightBrace) &&
        !isAtEnd()
    ) {
        function->body.push_back(statement());
    }

    consume(
        TokenType::RightBrace,
        "Expected '}' after function body"
    );

    return function;
}
std::unique_ptr<Statement> Parser::statement() {
    if (match(TokenType::If)) {
        return ifStatement();
    }
    if (check(TokenType::StringType) || check(TokenType::Int) || check(TokenType::Bool)) {
        return variableDeclaration();
    }
    if (check(TokenType::Identifier)) {
        if(current+1<tokens.size()&&tokens[current+1].type == TokenType::Equal) {
            return assignmentStatement();
        }
        return functionCall();
    }
    throw std::runtime_error("Unexpected statement at line " + std::to_string(peek().line));
}

std::unique_ptr<Statement>
Parser::variableDeclaration() {
    std::string type;
    if (
        match(TokenType::StringType)
    ) {
        type="string";
    } else if (match(TokenType::Int)) {
        type = "int";
    } else if (match(TokenType::Bool)) {
        type = "bool";
    } else {
        throw std::runtime_error("Expected variable type at line " + std::to_string(peek().line));
    }

    const Token& name = consume(
        TokenType::Identifier,
        "Expected variable name after type"
    );

    consume(
        TokenType::Equal,
        "Expected '=' after variable name"
    );

    auto initializer = expression();

    consume(
        TokenType::Semicolon,
        "Expected ';' after variable declaration"
    );

    return std::make_unique<VariableDeclaration>(
        type,
        name.lexeme,
        std::move(initializer)
    );
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

    auto call =
        std::make_unique<FunctionCall>(name.lexeme);

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
std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::String)) {
        return std::make_unique<StringLiteral>(previous().lexeme);
    }
    if (match(TokenType::Integer)) {
        return std::make_unique<IntegerLiteral>(
            std::stoi(previous().lexeme)
        );
    }
    if (match(TokenType::True)) {
        return std::make_unique<BooleanLiteral>(true);
    }
    if (match(TokenType::False)) {
        return std::make_unique<BooleanLiteral>(false);
    }
    if (match(TokenType::Identifier)) {
        return std::make_unique<IdentifierExpression>(previous().lexeme);
    }
    if (match(TokenType::LeftParen)) {
        auto expressionInside = expression();
        consume(TokenType::RightParen, "Expected ')' after expression");
        return expressionInside;
    }
    throw std::runtime_error("Expected expression at line " + std::to_string(peek().line));
}

std::unique_ptr<Expression> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<Expression> Parser::addition() {
    auto expression = multiplication();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        const std::string operation = previous().lexeme;
        auto right = multiplication();
        expression = std::make_unique<BinaryExpression>(std::move(expression), operation, std::move(right));
    }
    return expression;
}

std::unique_ptr<Expression> Parser::multiplication() {
    auto expression = unary();

    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        const std::string operation = previous().lexeme;
        auto right = unary();
        expression = std::make_unique<BinaryExpression>(std::move(expression), operation, std::move(right));
    }
    return expression;
}
std::unique_ptr<Statement> Parser::assignmentStatement() {
    const Token& name = consume(TokenType::Identifier, "Expected variable name");
    consume(TokenType::Equal, "Expected '=' after variable name");
    auto value = expression();
    consume(TokenType::Semicolon, "Expected ';' after assignment");
    return std::make_unique<AssignmentStatement> (
        name.lexeme,
        std::move(value)
    );
}
std::unique_ptr<Expression> Parser::equality() {
    auto expression = comparison();

    while (match(TokenType::EqualEqual)||match(TokenType::BangEqual)) {
        const std::string operation = previous().lexeme;
        auto right = comparison();
        expression = std::make_unique<BinaryExpression>(std::move(expression),operation,std::move(right));
    }
    return expression;
}
std::unique_ptr<Expression> Parser::comparison() {
    auto expression = addition();
    while (match(TokenType::Less)||match(TokenType::LessEqual)||match(TokenType::Greater)||match(TokenType::GreaterEqual)) {
        const std::string operation = previous().lexeme;
        auto right = addition();
        expression = std::make_unique<BinaryExpression>(std::move(expression),operation,std::move(right));
    }
    return expression;
}
std::unique_ptr<Expression> Parser::logicalOr() {
    auto expression = logicalAnd();
    while (match(TokenType::OrOr)) {
        const std::string operation = previous().lexeme;
        auto right = logicalAnd();
        expression = std::make_unique<BinaryExpression>(
            std::move(expression),
            operation,
            std::move(right)
        );
    }
    return expression;
}
std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expression = equality();
    while (match(TokenType::AndAnd)) {
        const std::string operation = previous().lexeme;
        auto right = equality();
        expression = std::make_unique<BinaryExpression>(
            std::move(expression),
            operation,
            std::move(right)
        );
    }
    return expression;
}
std::unique_ptr<Expression> Parser::unary() {
    if (match(TokenType::Bang)) {
        const std::string operation = previous().lexeme;
        auto operand = unary();
        return std::make_unique<UnaryExpression>(operation, std::move(operand));
    }
    return primary();
}
std::unique_ptr<Statement> Parser::ifStatement() {
    consume(
        TokenType::LeftParen,
        "Expected '(' after 'if'"
    );
    auto condition = expression();
    consume(
        TokenType::RightParen,
        "Expected ')' after if condition"
    );
    consume(
        TokenType::LeftBrace,
        "Exoected '{' before if body"
    );
    std::vector<std::unique_ptr<Statement>> body;

    while (
        !check(TokenType::RightBrace) &&
        !isAtEnd()
    ) {
        body.push_back(statement());
    }
    consume(
        TokenType::RightBrace,
        "Expected '}' after if body"
    );
    return std::make_unique<IfStatement>(
        std::move(condition),
        std::move(body)
    );
}