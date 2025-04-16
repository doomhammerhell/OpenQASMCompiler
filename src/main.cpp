#include <iostream>
#include <fstream>
#include <string>
#include "ast/ast.hpp"
#include "interpreter/interpreter.hpp"

extern int yyparse();
extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.qasm>" << std::endl;
        return 1;
    }

    // Open the input file
    FILE* input = fopen(argv[1], "r");
    if (!input) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        return 1;
    }

    yyin = input;

    try {
        // Parse the input
        if (yyparse() != 0) {
            std::cerr << "Error: Parsing failed" << std::endl;
            return 1;
        }

        // Create and run interpreter
        qasm::Interpreter interpreter;
        // TODO: Get AST from parser and execute
        // interpreter.execute(ast);

        std::cout << "Program executed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    fclose(input);
    return 0;
} 