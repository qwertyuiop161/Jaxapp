cmake_minimum_required(VERSION 3.20)

project(JAXAPP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(JAXAPP
    src/main.cpp
    src/Lexer.cpp
    src/Token.cpp
    src/Parser.cpp
    src/ASTPrinter.cpp
    src/Semantic.cpp
    src/CodeGen.cpp
)