#pragma once

#include <string>

#include "AST.h"

class CodeGenerator {
    public:
        std::string generate(const Program& program);
    private:
        std::string generateFunction(const FunctionDeclaration& function);
        std::string generateStatement(const Statement& statement);
        std::string generateExpression(const Expression& expression);
};
