#pragma once

#include <memory>
#include <string>
#include <utility>
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

class IntegerLiteral : public Expression {
    public:
        int value;
        explicit IntegerLiteral(int value) : value(value) {}
};
class BooleanLiteral : public Expression {
    public:
        bool value;
        explicit BooleanLiteral(bool value) : value(value) {}
};
class IdentifierExpression : public Expression {
    public:
        std::string name;
        explicit IdentifierExpression(const std::string& name) : name(name) {}
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

class VariableDeclaration : public Statement {
    public:
        std::string type;
        std::string name;
        std::unique_ptr<Expression> initializer;

        VariableDeclaration(
            const std::string& type,
            const std::string& name,
            std::unique_ptr<Expression> initializer
        ) : type(type), name(name), initializer(std::move(initializer)) {}
};

class FunctionDeclaration : public ASTNode {
    public:
        std::string name;
        std::vector<std::unique_ptr<Statement>> body;
        explicit FunctionDeclaration (const std::string& name) : name(name) {}
};

class Program : public ASTNode {
    public:
        std::vector<std::unique_ptr<FunctionDeclaration>> functions;
};

class BinaryExpression : public Expression {
    public:
        std::unique_ptr<Expression> left;
        std::string operation;
        std::unique_ptr<Expression> right;

        BinaryExpression(
            std::unique_ptr<Expression> left,
            const std::string& operation,
            std::unique_ptr<Expression> right
        ) : left(std::move(left)), operation(operation), right(std::move(right))
        {}
};
class UnaryExpression : public Expression {
    public:
        std::string operation;
        std::unique_ptr<Expression> operand;

        UnaryExpression(const std::string& operation, std::unique_ptr<Expression> operand) : operation(operation), operand(std::move(operand)) {}
};
class AssignmentStatement : public Statement {
    public:
        std::string name;
        std::unique_ptr<Expression> value;

        AssignmentStatement(const std::string& name, std::unique_ptr<Expression> value) : name(name), value(std::move(value)) {}
};