#pragma once

#include <memory>
#include <string>
#include <vector>

class ASTNode {
    public:
        virtual ~ASTNode() = default;
};
class Expression : public ASTNode {
    public:
        virtual ~Expression() = default;
};
class StringLiteral : public Expression {
    public:
        std::string value;
        explicit StringLiteral(const std::string& value) : value(value) {}
};
class Statement : public ASTNode {
    public:
        virtual ~Statement() = default;
};
class FunctionCall : public Statement {
    public:
        std::string name;
        std::vector<std::unique_ptr<Expression>> arguments;
        explicit FunctionCall(const std::string& name) : name(name) {}
};
class FunctionDeclaration : public ASTNode {
    public:
        std::string name;
        std::vector<std::unique_ptr<Statement>> body;
        explicit FunctionDeclaration(const std::string& name) : name(name) {}
};
class Program : public ASTNode {
    public:
        std::vector<std::unique_ptr<FunctionDeclaration>> functions;
};