#pragma once

#include "AST.h"

class SemanticAnalyzer {
    public:
        void analyze(const Program& program);
    private:
        void analyzeFunction(const FunctionDeclaration& function);
        void analyzeStatement(const Statement& statement);
        void analyzeExpression(const Expression& expression);
};
