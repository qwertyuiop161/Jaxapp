#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "AST.h"
#include "Token.h"

class Parser {
    public:
        explicit Parser(const std::vector<Token>& tokens);

        std::unique_ptr<Program> parse();
    private:
        const std::vector<Token>& tokens;
        size_t current = 0;

        bool isAtEnd() const;

        const Token& peek() const;
        const Token& previous() const;
        const Token& advance();

        bool check(TokenType type) const;
        bool match(TokenType type);

        const Token& consume(TokenType type, const std::string& message);

        std::unique_ptr<FunctionDeclaration> functionDeclaration();
        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> functionCall();
        std::unique_ptr<Expression> expression();
        std::unique_ptr<Expression> primary();
};
