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
        output += "void " + function.name + "(";
        for (std::size_t i = 0; i<function.parameters.size(); ++i) {
            const auto& parameter = function.parameters[i];
            if (parameter.type=="string") {
                output +="std::string";
            } else if (parameter.type == "int") {
                output+="int";
            } else if (parameter.type=="bool") {
                output+="bool";
            } else {
                throw std::runtime_error(
                    "Code generation error: unsupported parameter type '" + parameter.type + "'."
                );
            }
            output += " " + parameter.name;
            if (i+1<function.parameters.size()) {
                output+=", ";
            }
        }
        output+=")\n";
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
        } else if (variable->type == "bool") {
            cppType="bool";
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
        std::string output = call->name + "(";
        for (std::size_t i = 0; i<call->arguments.size(); ++i) {
            output+=generateExpression(*call->arguments[i]);
            if (i+1<call->arguments.size()) {
                output+=", ";
            }
        }
        output+=");";

        return output;
    }
    if (const auto* ifStatement = dynamic_cast<const IfStatement*>(&statement)) {
        std::string output;
        output+="if (";
        output+=generateExpression(*ifStatement->condition);
        output+=")\n";
        output+="   {\n";
        for (const auto& nestedStatement : ifStatement->thenBranch) {
            output+="       ";
            output+=generateStatement(*nestedStatement);
            output+="\n";
        }
        output+="   }";
        if (!ifStatement->elseBranch.empty()) {
            output+="\n";
            output+="   else\n";
            output+="   {\n";
            for (const auto& nestedStatement : ifStatement->elseBranch) {
                output+="       ";
                output+=generateStatement(*nestedStatement);
                output+="\n";
            }
            output+="   }";
        }
        return output;
    }
    if (const auto* whileStatement = dynamic_cast<const WhileStatement*>(&statement)) {
        std::string output;
        output+="while (";
        output+=generateExpression(*whileStatement->condition);
        output+=")\n";
        output+="   {\n";
        for (const auto& nestedStatement : whileStatement->body) {
            output+="       ";
            output+=generateStatement(*nestedStatement);
            output+="\n";
        }
        output+="   }";
        return output;
    }
    if (const auto* forStatement = dynamic_cast<const ForStatement*>(&statement)) {
        std::string output;
        std::string initializer = generateStatement(*forStatement->initializer);
        if (!initializer.empty() && initializer.back() == ';') {
            initializer.pop_back();
        }
        std::string increment = generateStatement(*forStatement->increment);
        if (!increment.empty() && increment.back()==';') {
            increment.pop_back();
        }
        output+="for (";
        output+=initializer;
        output+="; ";
        output+=generateExpression(*forStatement->condition);
        output+="; ";
        output+=increment;
        output+=")\n";
        output+="   {\n";
        for (const auto& nestedStatement:forStatement->body) {
            output+="       ";
            output+=generateStatement(*nestedStatement);
            output+="\n";
        }
        output+="   }";
        return output;
    }
    if (dynamic_cast<const BreakStatement*>(&statement)) {
        return "break;";
    }
    if (dynamic_cast<const ContinueStatement*>(&statement)) {
        return "continue;";
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
    if (const auto* booleanLiteral = dynamic_cast<const BooleanLiteral*>(&expression)) {
        return booleanLiteral->value
            ?"true"
            :"false";
    }
    if (const auto* identifier =
            dynamic_cast<const IdentifierExpression*>(&expression)) {
        return identifier->name;
    }
    if (const auto* binary = dynamic_cast<const BinaryExpression*>(&expression)) {
        return "(" + generateExpression(*binary->left) + " " + binary->operation + " " + generateExpression(*binary->right) + ")";
    }
    if (const auto* unary = dynamic_cast<const UnaryExpression*>(&expression)) {
        return "(" + unary->operation + generateExpression(*unary->operand)+")";
    }
    throw std::runtime_error(
        "Code generation error: unsupported expression."
    );
}