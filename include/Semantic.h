#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

#include "AST.h"

class SemanticAnalyzer {
    public:
        void analyze(const Program& program);
    private:
        std::vector<std::unordered_map<std::string, std::string>> scopes;
        std::string currentReturnType;
        struct FunctionInfo {
            std::vector<std::string> parameterTypes;
            std::string returnType;
        };
        std::unordered_map<std::string,FunctionInfo> functions;
        int loopDepth=0;
        void analyzeFunction(const FunctionDeclaration& function);

        void analyzeStatement(const Statement& statement);

        void beginScope();
        bool statementAlwaysReturns(const Statement& statement) const;
        void endScope();

        void declareVariable(const std::string& name, const std::string& type);
        const std::string* findVariable(const std::string& name) const;

        std::string analyzeExpression(const Expression& expression);
};