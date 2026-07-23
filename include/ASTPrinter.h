#pragma once

#include "AST.h"

class ASTPrinter {
    public:
        void print(const Program& program);
    private:
        void printFunction(const FunctionDeclaration& function);
        void printStatement(const Statement& statement, int indent);
        void printExpression(const Expression& expression, int indent);

        void printIndent(int indent);
};
