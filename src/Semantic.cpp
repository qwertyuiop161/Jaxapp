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
    if (const auto* variable = dynamic_cast<const VariableDeclaration*>(&statement)) {
        if (variable->type!="string") {
            throw std::runtime_error("Semantic error: unsupported variable type '" + variable->type + "'.");
        }
        if (!variable->initializer) {
            throw std::runtime_error("Semantic error: variable '" + variable->name + "' requires an initializer.");
        }
        analyzeExpression(*variable->initializer);
        return;
    }
    if (const auto* call = dynamic_cast<const FunctionCall*>(&statement)) {
        if (call->name != "print") {
            throw std::runtime_error("Semantic error: unknown function '" + call->name + "'.");
        }
        if (call->arguments.size()!=1) {
            throw std::runtime_error("Semantic error: print requires exactly one argument.");
        }
        analyzeExpression(*call->arguments[0]);
        return;
    }
    throw std::runtime_error("Semantic error: unknown statement");
}
void SemanticAnalyzer::analyzeExpression(const Expression& expression) {
    if (dynamic_cast<const StringLiteral*>(&expression)) {
        return;
    }
    if (dynamic_cast<const IdentifierExpression*>(&expression)) {
        return;
    }
    throw std::runtime_error("Semantic error: unsupported expression.");
}