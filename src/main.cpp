#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"

int main() {
    std::ifstream file("../examples/hello.jx");
    if (!file.is_open()) {
        std::cout << "Could not open hello.jx\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parse();

    std::cout << "Program\n";
    for (const auto& function : program->functions) {
        std::cout<< "  Function: " << function->name << "\n";
        for (const auto& statement : function->body) {
            if (auto* call = dynamic_cast<FunctionCall*>(statement.get())) {
                std::cout << "   Call: " << call->name << "\n";
                for (const auto& argument : call->arguments) {
                    if (auto* stringLiteral = dynamic_cast<StringLiteral*>(argument.get())) {
                        std::cout << "    String: \""
                                  << stringLiteral->value
                                  << "\"\n";
                    }
                }
            }
        }
    }
    
    return 0;
}