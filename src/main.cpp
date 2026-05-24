// src/main.cpp
#include "core/lexer.h"
#include "core/parser.h"
#include "runtime/vm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void runFile(const std::string& path) {
    std::string source = readFile(path);
    
    nova::Lexer lexer(source);
    std::vector<nova::Token> tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        if (token.type == nova::TokenType::ERROR) {
            std::cerr << "Lexer error: " << token.lexeme << std::endl;
            return;
        }
    }
    
    nova::Parser parser(tokens);
    auto statements = parser.parse();
    
    nova::VM vm;
    vm.interpret(statements);
}

void runRepl() {
    std::cout << "Nova Programming Language REPL" << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;
    
    nova::VM vm;
    
    while (true) {
        std::cout << ">>> ";
        std::string line;
        std::getline(std::cin, line);
        
        if (line == "exit") break;
        if (line.empty()) continue;
        
        nova::Lexer lexer(line);
        std::vector<nova::Token> tokens = lexer.tokenize();
        
        nova::Parser parser(tokens);
        auto statements = parser.parse();
        
        vm.interpret(statements);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        runRepl();
    } else if (argc == 2) {
        try {
            runFile(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Usage: nova [script]" << std::endl;
        return 64;
    }
    
    return 0;
}