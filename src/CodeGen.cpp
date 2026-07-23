#include "CodeGen.h"

#include <stdexcept>

std::string CodeGenerator::generate(const Program& program) {
    std::string output;
    output+="#include <iostream>\n";
    output+="#include <string>\n\n";
    for (const auto& function : program.functions) {
        output+=generateFunction(*function);
        output+="\n";
    }
    return output;
}
std::string CodeGenerator::generateFunction(const FunctionDeclaration& function) {
    std::string output;
    if (function.name == "main") {
        output+="int main()\n";
    } else {
        output+="void "+function.name+"()\n";
    }
    output+="{\n";
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
    if (const auto* call = dynamic_cast<const FunctionCall*>(&statement)) {
        if (call->name == "print") {
            if (call->arguments.size() != 1) {
                throw std::runtime_error("Code generation error: print requires one argument.");
            }
            return "std::cout << " + generateExpression(*call->arguments[0]) + " << '\\n';";
        }
        throw std::runtime_error("Code generation error: unsupported function call.");
    }
    throw std::runtime_error("Code generation error: unsupported statement.");
}
std::string CodeGenerator::generateExpression(const Expression& expression) {
    if (const auto* stringLiteral = dynamic_cast<const StringLiteral*>(&expression)) {
        return "\"" + stringLiteral->value +"\"";
    }
    throw std::runtime_error("Code generation error: unsupported expression");
}
