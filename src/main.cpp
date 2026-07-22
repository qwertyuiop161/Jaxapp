#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"

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

    ASTPrinter printer;
    printer.print(*program);
    
    return 0;
}