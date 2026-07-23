#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Semantic.h"
#include "CodeGen.h"

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

    SemanticAnalyzer semantic;
    semantic.analyze(*program);

    CodeGenerator codeGenerator;
std::string generatedCode = codeGenerator.generate(*program);

const std::filesystem::path outputPath =
    std::filesystem::absolute("generated.cpp");

std::cout << "\nGenerated text before writing:\n";
std::cout << "--------------------------------\n";
std::cout << generatedCode;
std::cout << "--------------------------------\n";
std::cout << "Writing to: " << outputPath << '\n';
std::cout << "Generated size: " << generatedCode.size() << " bytes\n";

std::ofstream outputFile(
    outputPath,
    std::ios::out | std::ios::binary | std::ios::trunc
);

if (!outputFile) {
    std::cerr << "Could not create: " << outputPath << '\n';
    return 1;
}

outputFile.write(
    generatedCode.data(),
    static_cast<std::streamsize>(generatedCode.size())
);

if (!outputFile) {
    std::cerr << "Failed while writing generated code.\n";
    return 1;
}

outputFile.close();

std::cout << "Successfully generated: " << outputPath << '\n';

    ASTPrinter printer;
    printer.print(*program);
    
    return 0;
}
