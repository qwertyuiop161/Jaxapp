#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
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
    for (const auto& token : tokens) {
        std::cout<<token.lexeme<<"  "<<static_cast<int>(token.type)<<"\n";
    }
    return 0;
}