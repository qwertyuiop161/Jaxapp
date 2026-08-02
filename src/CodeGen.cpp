#include "CodeGen.h"

#include <stdexcept>

std::string CodeGenerator::generate(
    const Program& program
) {
    std::string output;

    output += "#include <iostream>\n";
    output += "#include <string>\n\n";

    for (const auto& function : program.functions) {
        if (function->name != "main") {
            output += "void " + function->name + "();\n";
        }
    }

    output += "\n";

    for (const auto& function : program.functions) {
        output += generateFunction(*function);
        output += "\n";
    }

    return output;
}
std::string CodeGenerator::generateFunction(
    const FunctionDeclaration& function
) {
    std::string output;

    if (function.name == "main") {
        output += "int main()\n";
    } else {
        output += "void " + function.name + "()\n";
    }

    output += "{\n";

    for (const auto& statement : function.body) {
        output += "    ";
        output += generateStatement(*statement);
        output += "\n";
    }

    if (function.name == "main") {
        output += "    return 0;\n";
    }

    output += "}\n";

    return output;
}
std::string CodeGenerator::generateStatement(const Statement& statement) {
    if (const auto* variable = dynamic_cast<const VariableDeclaration*>(&statement)) {
        std::string cppType;

        if (variable->type=="string") {
            cppType="std::string";
        } else if (variable->type == "int") {
            cppType="int";
        } else {
            throw std::runtime_error("Code generation error: unsupported variable type '" + variable->type + "'.");
        }
        return cppType + " " + variable->name + " = " + generateExpression(*variable->initializer) + ";";
    }
    if (const auto* assignment = dynamic_cast<const AssignmentStatement*>(&statement)) {
        return assignment->name + " = " + generateExpression(*assignment->value) + ";";
    }
    if (const auto* call =
            dynamic_cast<const FunctionCall*>(&statement)) {
        if (call->name == "print") {
            if (call->arguments.size() != 1) {
                throw std::runtime_error(
                    "Code generation error: print requires one argument."
                );
            }

            return "std::cout << " +
                   generateExpression(*call->arguments[0]) +
                   " << '\\n';";
        }

        if (!call->arguments.empty()) {
            throw std::runtime_error(
                "Code generation error: user-defined function arguments "
                "are not supported yet."
            );
        }

        return call->name + "();";
    }

    throw std::runtime_error(
        "Code generation error: unsupported statement."
    );
}

std::string CodeGenerator::generateExpression(
    const Expression& expression
) {
    if (const auto* stringLiteral =
            dynamic_cast<const StringLiteral*>(&expression)) {
        return "\"" + stringLiteral->value + "\"";
    }
    if (const auto* integerLiteral = dynamic_cast<const IntegerLiteral*>(&expression)) {
        return std::to_string(integerLiteral->value);
    }

    if (const auto* identifier =
            dynamic_cast<const IdentifierExpression*>(&expression)) {
        return identifier->name;
    }
    if (const auto* binary = dynamic_cast<const BinaryExpression*>(&expression)) {
        return "(" + generateExpression(*binary->left) + " " + binary->operation + " " + generateExpression(*binary->right) + ")";
    }
    throw std::runtime_error(
        "Code generation error: unsupported expression."
    );
}