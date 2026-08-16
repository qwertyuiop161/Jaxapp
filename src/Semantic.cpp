#include "Semantic.h"

#include <stdexcept>
void SemanticAnalyzer::beginScope() {
    scopes.emplace_back();
}
void SemanticAnalyzer::endScope() {
    scopes.pop_back();
}
void SemanticAnalyzer::declareVariable(const std::string& name,const std::string& type) {
    if (scopes.empty()) {
        beginScope();
    }
    auto& currentScope = scopes.back();
    if (currentScope.contains(name)) {
        throw std::runtime_error("Semantic error: variable '" + name + "' is already declared in this scope.");
    }
    currentScope.emplace(name, type);
}
const std::string* SemanticAnalyzer::findVariable(const std::string& name) const {
    for (auto it = scopes.rbegin(); it!=scopes.rend(); ++it) {
        const auto found = it->find(name);
        if (found!=it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}
void SemanticAnalyzer::analyze(const Program& program) {
    bool foundMain = false;
    functions.clear();

    for (const auto& function : program.functions) {
        if (functions.contains(function->name)) {
            throw std::runtime_error("Semantic error: function '" + function->name + "' is already declared.");
        }
        functions.insert(function->name);
        if (function->name == "main") {
            if (foundMain) {
                throw std::runtime_error("Semantic error: multiple main functions.");
            }
            foundMain = true;
        }
    }
    if (!foundMain) {
        throw std::runtime_error("Semantic error: program must contain a main function.");
    }
    for (const auto& function : program.functions) {
        analyzeFunction(*function);
    }
}
void SemanticAnalyzer::analyzeFunction(const FunctionDeclaration& function) {
    scopes.clear();
    beginScope();
    for (const auto& statement : function.body) {
        analyzeStatement(*statement);
    }
    endScope();
}
void SemanticAnalyzer::analyzeStatement(const Statement& statement) {
    if (const auto* variable = dynamic_cast<const VariableDeclaration*>(&statement)) {
        if (!variable->initializer) {
            throw std::runtime_error("Semantic error: variable'" + variable->name + "' requires an initialier.");
        }
        const std::string initializerType = analyzeExpression(*variable->initializer);
        if (initializerType!=variable->type) {
            throw std::runtime_error("Semantic error: cannot initialize variable '" + variable->name + "' of type '" + variable->type + "' with an expression of type '" + initializerType + "'.");
        }
        declareVariable(variable->name,variable->type);
        return;
    }
    if (const auto* assignment = dynamic_cast<const AssignmentStatement*>(&statement)) {
        const std::string* variableType=findVariable(assignment->name);

        if (variableType == nullptr) {
            throw std::runtime_error("Semantic error: undefined variable '" + assignment->name+"'.");
        }
        const std::string valueType = analyzeExpression(*assignment->value);
        if (valueType!=*variableType) {
            throw std::runtime_error("Semantic error: cannot assign expression of type '" + valueType + "' to variable '" + assignment->name + "' of type '" + *variableType + "'.");
        }
        return;
    }
    if (const auto* call = dynamic_cast<const FunctionCall*>(&statement)) {
        if (call->name=="print") {
            if (call->arguments.size()!=1) {
                throw std::runtime_error("Semantic error: print requires exactly one argument.");
            }
            analyzeExpression(*call->arguments[0]);
            return;
        }
        if (!functions.contains(call->name)) {
            throw std::runtime_error("Semantic error: unknown function '" + call->name + "'.");
        }
        if (!call->arguments.empty()) {
            throw std::runtime_error("Semantic error: function '" + call->name + "' does not accept arguments.");
        }
        return;
    }
    if (const auto* ifStatement = dynamic_cast<const IfStatement*>(&statement)) {
        const std::string conditionType = analyzeExpression(*ifStatement->condition);

        if (conditionType!="bool") {
            throw std::runtime_error("Semantic error: if condition must be bool.");
        }
        beginScope();
        for (const auto& nestedStatement : ifStatement->thenBranch) {
            analyzeStatement(*nestedStatement);
        }
        endScope();
        if (!ifStatement->elseBranch.empty()) {
            beginScope();
            for (const auto& nestedStatement : ifStatement->elseBranch) {
                analyzeStatement(*nestedStatement);
            }
            endScope();
        }
        return;
    }
    if (const auto* whileStatement = dynamic_cast<const WhileStatement*>(&statement)) {
        const std::string conditionType = analyzeExpression(*whileStatement->condition);
        if (conditionType!="bool") {
            throw std::runtime_error("Semantic error: while condition must be bool");
        }
        beginScope();
        for (const auto& nestedStatement : whileStatement->body) {
            analyzeStatement(*nestedStatement);
        }
        endScope();
        return;
    }
    throw std::runtime_error("Semantic error: unknown statement.");
}
std::string SemanticAnalyzer::analyzeExpression(const Expression& expression) {
    if (dynamic_cast<const StringLiteral*>(&expression)) {
        return "string";
    }
    if (dynamic_cast<const BooleanLiteral*>(&expression)) {
        return "bool";
    }
    if (dynamic_cast<const IntegerLiteral*>(&expression)) {
        return "int";
    }
    if (const auto* identifier = dynamic_cast<const IdentifierExpression*>(&expression)) {
        const std::string* variableType = findVariable(identifier->name);
        if (variableType==nullptr) {
            throw std::runtime_error("Semantic error: undefined variable '" + identifier->name + "'.");
        }
        return *variableType;
    }
    if (const auto* binary = dynamic_cast<const BinaryExpression*>(&expression)) {
        const std::string leftType = analyzeExpression(*binary->left);
        const std::string rightType = analyzeExpression(*binary->right);
        const std::string& op = binary->operation;

        if (op=="+"||op=="-"||op=="*"||op=="/"||op=="%") {
            if (leftType!="int"||rightType!="int") {
                throw std::runtime_error("Semantic error: operator '" + op + "' requires integer operands.");
            }
            return "int";
        }
        if (op=="<"||op=="<="||op==">"||op==">=") {
            if (leftType!="int"||rightType!="int") {
                throw std::runtime_error("Semantic error: operator '" + op + "' requires integer operands.");
            }
            return "bool";
        }
        if (op=="=="||op=="!=") {
            if (leftType!=rightType) {
                throw std::runtime_error("Semantic error: operator '" + op + "' requires operands of the same type.");
            }
            return "bool";
        }
        if (op=="&&"||op=="||") {
            if (leftType!="bool"||rightType!="bool") {
                throw std::runtime_error("Semantic error: operator '" + op + "' requires boolean operands.");
            }
            return "bool";
        }
        throw std::runtime_error("Semantic error: unsupported binary operator '" + op + "'.");
    }
    if (const auto* unary = dynamic_cast<const UnaryExpression*>(&expression)) {
        const std::string operandType = analyzeExpression(*unary->operand);
        if (unary->operation == "!") {
            if (operandType!="bool") {
                throw std::runtime_error("Semantic error: operator '!' requires a boolean operand.");
            }
            return "bool";
        }
        throw std::runtime_error("Semantic error: unsupported unary operator '" + unary->operation + "'.");
    }
    throw std::runtime_error("Semantic error: unsupported expression.");
}