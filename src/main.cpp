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

    std::cout << "Parsed "
              << program->functions.size()
              << "function(s).\n";
    
    return 0;
}