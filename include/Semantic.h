#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "AST.h"

class SemanticAnalyzer {
    public:
        void analyze(const Program& program);
    private:
        std::vector<std::unordered_map<std::string, std::string>> scopes;
        std::unordered_set<std::string> functions;

        void analyzeFunction(const FunctionDeclaration& function);

        void analyzeStatement(const Statement& statement);

        void beginScope();
        void endScope();

        void declareVariable(const std::string& name, const std::string& type);
        const std::string* findVariable(const std::string& name) const;

        std::string analyzeExpression(const Expression& expression);
};