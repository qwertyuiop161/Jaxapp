#include "Semantic.h"

#include <stdexcept>

void SemanticAnalyzer::analyze(const Program& program) {
    bool foundMain = false;

    for (const auto& function : program.functions) {
        if (function->name == "main") {
            if (foundMain) {
                throw std::runtime_error("Semantic error: multiple main functions.");
            }
            foundMain = true;
        }
        analyzeFunction(*function);
    }
    if (!foundMain) {
        throw std::runtime_error("Semantic error: program must contain a main function.");
    }
}
void SemanticAnalyzer::analyzeFunction(const FunctionDeclaration& function) {
    for (const auto& statement : function.body) {
        analyzeStatement(*statement);
    }
}
void SemanticAnalyzer::analyzeStatement(const Statement& statement) {
    if (const auto* call = dynamic_cast<const FunctionCall*>(&statement)) {
        if (call->name != "print") {
            throw std::runtime_error("Semantic error: unknown function '" + call->name + "'.");
        }
        if (call->arguments.size() != 1) {
            throw std::runtime_error("Semantic error: print requires exactly one argument");
        }
        for (const auto& argument : call->arguments) {
            analyzeExpression(*argument);
        }
        return;
    }
    throw std::runtime_error("Semantic error: unknown statement.");
}
void SemanticAnalyzer::analyzeExpression(const Expression& expression) {
    if (dynamic_cast<const StringLiteral*>(&expression)) {
        return;
    }
    throw std::runtime_error("Semantic error: unsupported expression.");
}
