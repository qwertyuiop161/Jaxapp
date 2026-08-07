#include "ASTPrinter.h"

#include <iostream>

void ASTPrinter::print(const Program& program) {
    std::cout << "Program\n";
    for (const auto& function : program.functions) {
        printFunction(*function);
    }
}
void ASTPrinter::printFunction(const FunctionDeclaration& function) {
    std::cout << " Function: " << function.name << "\n";
    for (const auto& statement : function.body) {
        printStatement(*statement, 2);
    }
}
void ASTPrinter::printStatement(
    const Statement& statement,
    int indent
) {
    if (const auto* variable = dynamic_cast<const VariableDeclaration*>(&statement)) {
        printIndent(indent);
        std::cout<<"Variable: "<<variable->type<<" "<<variable->name<<"\n";
        if (variable->initializer) {
            printExpression(*variable->initializer, indent+1);
        }
        return;
    }
    if (const auto* assignment = dynamic_cast<const AssignmentStatement*>(&statement)) {
        printIndent(indent);
        std::cout<<"Assignment: "
                 << assignment->name
                 <<"\n";
        printExpression(*assignment->value, indent+1);
        return;
    }
    if (const auto* call = dynamic_cast<const FunctionCall*>(&statement)) {
        printIndent(indent);
        std::cout << "Call: " << call->name << "\n";
        for (const auto& argument : call->arguments) {
            printExpression(*argument, indent + 1);
        }
        return;
    }
    printIndent(indent);
    std::cout << "Unknown statement\n";
}
void ASTPrinter::printExpression(
    const Expression& expression,
    int indent
) {
    if (const auto* stringLiteral = dynamic_cast<const StringLiteral*>(&expression)) {
        printIndent(indent);
        std::cout << "String: \""
                  << stringLiteral->value
                  <<"\"\n";
        return;
    }
    if (const auto* integerLiteral = dynamic_cast<const IntegerLiteral*>(&expression)) {
        printIndent(indent);
        std::cout << "Integer: "
                  << integerLiteral->value
                  << "\n";
        return;
    }
    if (const auto* booleanLiteral = dynamic_cast<const BooleanLiteral*>(&expression)) {
        printIndent(indent);
        std::cout << "Boolean: "
                  << (booleanLiteral->value ? "true" : "false")
                  << "\n";
        return;
    }
    if (const auto* identifier = dynamic_cast<const IdentifierExpression*>(&expression)) {
        printIndent(indent);
        std::cout << "Identifier: "
                  << identifier->name
                  <<"\n";
        return;
    }
    printIndent(indent);
    if (const auto* binary = dynamic_cast<const BinaryExpression*>(&expression)) {
        printIndent(indent);
        std::cout << "Binary: "
                  << binary->operation
                  << "\n";
        printExpression(*binary->left, indent+1);
        printExpression(*binary->right,indent+1);
        return;
    }
    if (const auto* unary = dynamic_cast<const UnaryExpression*>(&expression)) {
        printIndent(indent);
        std::cout<<"Unary: "<<unary->operation<<"\n";
        printExpression(*unary->operand,indent+1);
        return;
    }
    std::cout << "Unknown expression\n";
}
void ASTPrinter::printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << " ";
    }
}
