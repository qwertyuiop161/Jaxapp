#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "AST.h"

class SemanticAnalyzer {
    public:
        void analyze(const Program& program);
    private:
        std::unordered_map<std::string, std::string> variables;
        std::unordered_set<std::string> functions;

        void analyzeFunction(const FunctionDeclaration& function);

        void analyzeStatement(const Statement& statement);

        std::string analyzeExpression(const Expression& expression);
};