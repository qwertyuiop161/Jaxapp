#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Semantic.h"
#include "CodeGen.h"

int main() {
    try {
        const std::filesystem::path sourcePath =
            std::filesystem::absolute("../examples/hello.jx");

        std::ifstream file(sourcePath);

        if (!file.is_open()) {
            std::cerr << "Could not open: "
                      << sourcePath
                      << '\n';
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        const std::string source = buffer.str();

        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto program = parser.parse();

        SemanticAnalyzer semantic;
        semantic.analyze(*program);

        CodeGenerator codeGenerator;
        const std::string generatedCode =
            codeGenerator.generate(*program);

        const std::filesystem::path outputPath =
            std::filesystem::absolute("generated.cpp");

        std::ofstream outputFile(
            outputPath,
            std::ios::out |
            std::ios::binary |
            std::ios::trunc
        );

        if (!outputFile.is_open()) {
            std::cerr << "Could not create: "
                      << outputPath
                      << '\n';
            return 1;
        }

        outputFile.write(
            generatedCode.data(),
            static_cast<std::streamsize>(
                generatedCode.size()
            )
        );

        if (!outputFile) {
            std::cerr << "Failed while writing generated code\n";
            return 1;
        }

        outputFile.close();

        ASTPrinter printer;
        printer.print(*program);

        std::cout << "\nGenerated: "
                  << outputPath
                  << '\n';

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr << "Compiler error: "
                  << exception.what()
                  << '\n';
        return 1;
    }
}