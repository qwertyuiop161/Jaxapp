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
    if (const auto* identifier = dynamic_cast<const IdentifierExpression*>(&expression)) {
        printIndent(indent);
        std::cout << "Identifier: "
                  << identifier->name
                  <<"\n";
        return;
    }
    printIndent(indent);
    std::cout << "Unknown expression\n";
}
void ASTPrinter::printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << " ";
    }
}
