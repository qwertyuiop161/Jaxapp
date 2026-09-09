#include "Semantic.h"
#include <iostream>
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
    functions.clear();

    for (const auto& function : program.functions) {
        FunctionInfo info;
        info.returnType = function->returnType;
        for (const auto& parameter:function->parameters) {
            info.parameterTypes.push_back(parameter.type);
        }
        functions[function->name]=info;
    }
    for (const auto& function : program.functions) {
        analyzeFunction(*function);
    }
}
void SemanticAnalyzer::analyzeFunction(
    const FunctionDeclaration& function
) {
    currentReturnType = function.returnType;
    scopes.clear();
    beginScope();
    for (const auto& parameter : function.parameters) {
        declareVariable(parameter.name, parameter.type);
    }
    for (const auto& statement : function.body) {
        analyzeStatement(*statement);
    }
    if (currentReturnType != "void") {
        if (function.body.empty() || !statementAlwaysReturns(*function.body.back())) {
            throw std::runtime_error("Semantic error: function '" + function.name + "' must return a value on every path.");
        }
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
    if (const auto* call =
        dynamic_cast<const FunctionCall*>(&statement)) {

    if (call->name == "print") {
        if (call->arguments.size() != 1) {
            throw std::runtime_error(
                "Semantic error: print() expects exactly 1 argument."
            );
        }

        analyzeExpression(*call->arguments[0]);

        return;
    }

    auto functionIt = functions.find(call->name);

    if (functionIt == functions.end()) {
        throw std::runtime_error(
            "Semantic error: undefined function '" +
            call->name +
            "'."
        );
    }

    const auto& parameters =
        functionIt->second.parameterTypes;

    if (call->arguments.size() != parameters.size()) {
        throw std::runtime_error(
            "Semantic error: function '" +
            call->name +
            "' expects " +
            std::to_string(parameters.size()) +
            " argument(s), but got " +
            std::to_string(call->arguments.size()) +
            "."
        );
    }

    for (std::size_t i = 0;
         i < call->arguments.size();
         ++i) {

        const std::string argumentType =
            analyzeExpression(*call->arguments[i]);

        if (argumentType != parameters[i]) {
            throw std::runtime_error(
                "Semantic error: argument " +
                std::to_string(i + 1) +
                " of function '" +
                call->name +
                "' must be '" +
                parameters[i] +
                "', but got '" +
                argumentType +
                "'."
            );
        }
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
        loopDepth++;
        for (const auto& nestedStatement : whileStatement->body) {
            analyzeStatement(*nestedStatement);
        }
        loopDepth--;
        endScope();
        return;
    }
    if (const auto* forStatement = dynamic_cast<const ForStatement*>(&statement)) {
        beginScope();
        if (forStatement->initializer) {
            analyzeStatement(*forStatement->initializer);
        }
        const std::string conditionType = analyzeExpression(*forStatement->condition);
        if(conditionType!="bool") {
            endScope();
            throw std::runtime_error("Semantic error: for condition must be bool.");
        }
        loopDepth++;
        for (const auto& nestedStatement : forStatement->body) {
            analyzeStatement(*nestedStatement);
        }
        if (forStatement->increment) {
            analyzeStatement(*forStatement->increment);
        }
        loopDepth--;
        endScope();
        return;
    }
    if (dynamic_cast<const BreakStatement*>(&statement)) {
        if (loopDepth==0) {
            throw std::runtime_error("Semantic error: 'break' can only be used inside a loop.");
        }
        return;
    }
    if (dynamic_cast<const ContinueStatement*>(&statement)) {
        if (loopDepth==0) {
            throw std::runtime_error("Semantic error: 'continue' can only be used inside a loop");
        }
        return;
    }
    if (const auto& returnStatement = dynamic_cast<const ReturnStatement*>(&statement)) {
        if (currentReturnType=="void") {
            throw std::runtime_error("Semantic error: void functions cannot return a value;");
        }
        if (!returnStatement->value) {
            throw std::runtime_error("Semantic error: non-void functions must return a value.");
        }
        const std::string valueType = analyzeExpression(*returnStatement->value);
        if (valueType!=currentReturnType) {
            throw std::runtime_error("Semantic error: function returns '" + currentReturnType + "', but returned expression has type '" + valueType + "'.");
        }
        return;
    }
    throw std::runtime_error("Semantic error: unknown statement.");
}
bool SemanticAnalyzer::statementAlwaysReturns(const Statement& statement) const {
    if (dynamic_cast<const ReturnStatement*>(&statement)) {
        return true;
    }
    if (const auto* ifStatement = dynamic_cast<const IfStatement*>(&statement)) {
        if (ifStatement->elseBranch.empty()) {
            return false;
        }
        return !ifStatement->thenBranch.empty() &&
        statementAlwaysReturns(*ifStatement->thenBranch.back()) &&
        statementAlwaysReturns(*ifStatement->elseBranch.back());
    }
    return false;
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
    if (const auto* call = dynamic_cast<const FunctionCall*>(&expression)) {
        if (call->name == "print") {
            if (call->arguments.size() != 1) {
                throw std::runtime_error("Semantic error: print() expects exactly one argument.");
            }
            analyzeExpression(*call->arguments[0]);
            return "void";
        }
        auto functionIt = functions.find(call->name);
        if (functionIt == functions.end()) {
            throw std::runtime_error("Semantic error: undefined function '" + call->name + "'.");
        }
        const auto& parameters = functionIt->second.parameterTypes;
        if (call->arguments.size() != parameters.size()) {
            throw std::runtime_error("Semantic error: function '" + call->name + "' expects " + std::to_string(parameters.size()) + "argument(s), but got " + std::to_string(call->arguments.size()) + ".");
        }
        for (std::size_t i = 0; i<call->arguments.size(); ++i) {
            const std::string argumentType = analyzeExpression(*call->arguments[i]);

            if (argumentType != parameters[i]) {
                throw std::runtime_error("Semantic error: argument " + std::to_string(i + 1) + " of function '" + call->name + "' must be '" + parameters[i] + "', but got '" + argumentType + "'.");
            }
        }
        return functionIt->second.returnType;
    }
    throw std::runtime_error("Semantic error: unsupported expression.");
}