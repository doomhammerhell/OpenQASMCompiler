#include "OpenQASMCompiler/cli.hpp"
#include "OpenQASMCompiler/parser.hpp"
#include "OpenQASMCompiler/ast.hpp"
#include "OpenQASMCompiler/ir.hpp"
#include <CLI/CLI.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace qasm {

/**
 * @brief Supported output formats
 */
enum class OutputFormat {
    IR,     // Intermediate Representation
    QASM,   // OpenQASM 2.0
    JSON,   // JSON format
    DOT,    // Graphviz DOT format
    CIRCUIT // Circuit diagram (ASCII)
};

/**
 * @brief Convert string to OutputFormat
 * @param format String representation of the format
 * @return OutputFormat enum value
 * @throw std::invalid_argument if format is invalid
 */
OutputFormat stringToFormat(const std::string& format) {
    if (format == "ir") return OutputFormat::IR;
    if (format == "qasm") return OutputFormat::QASM;
    if (format == "json") return OutputFormat::JSON;
    if (format == "dot") return OutputFormat::DOT;
    if (format == "circuit") return OutputFormat::CIRCUIT;
    throw std::invalid_argument("Invalid output format: " + format);
}

/**
 * @brief Parse command line arguments and execute the requested operations
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, non-zero on error
 */
int parseCommandLine(int argc, char** argv) {
    CLI::App app{"OpenQASM Compiler - A quantum circuit compiler and optimizer"};

    // Input file path
    std::string input_file;
    app.add_option("--input,-i", input_file, "Input QASM file path")
        ->required()
        ->check(CLI::ExistingFile);

    // Output file path
    std::string output_file;
    app.add_option("--output,-o", output_file, "Output file path");

    // Output format
    std::string format_str = "ir";
    app.add_option("--format,-f", format_str, "Output format (ir, qasm, json, dot, circuit)")
        ->check(CLI::IsMember({"ir", "qasm", "json", "dot", "circuit"}));

    // Dump AST flag
    bool dump_ast = false;
    app.add_flag("--dump-ast,-d", dump_ast, "Dump AST to console");

    // Optimization level
    int optimization_level = 0;
    app.add_option("--optimize,-O", optimization_level, "Optimization level (0-3)")
        ->check(CLI::Range(0, 3));

    // Verbose output
    bool verbose = false;
    app.add_flag("--verbose,-v", verbose, "Enable verbose output");

    // Validate circuit
    bool validate = false;
    app.add_flag("--validate", validate, "Validate circuit before processing");

    // Show gate statistics
    bool show_stats = false;
    app.add_flag("--stats", show_stats, "Show gate statistics");

    // Parse command line arguments
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    try {
        // Read input file
        std::ifstream file(input_file);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open input file: " + input_file);
        }

        std::string source((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

        if (verbose) {
            std::cout << "Parsing QASM source from: " << input_file << std::endl;
        }

        // Parse QASM source
        Parser parser;
        auto ast = parser.parse(source);
        if (!ast) {
            throw std::runtime_error("Failed to parse QASM source");
        }

        // Dump AST if requested
        if (dump_ast) {
            std::cout << "\nAbstract Syntax Tree:" << std::endl;
            std::cout << "====================" << std::endl;
            ast->dump();
            std::cout << "====================" << std::endl;
        }

        // Generate IR
        auto ir = ast->generateIR();
        if (!ir) {
            throw std::runtime_error("Failed to generate IR");
        }

        // Validate circuit if requested
        if (validate) {
            if (verbose) {
                std::cout << "Validating circuit..." << std::endl;
            }
            if (!ir->validate()) {
                throw std::runtime_error("Circuit validation failed");
            }
        }

        // Apply optimizations if requested
        if (optimization_level > 0) {
            if (verbose) {
                std::cout << "Applying optimizations (level " << optimization_level << ")..." << std::endl;
            }
            ir->optimize(optimization_level);
        }

        // Show gate statistics if requested
        if (show_stats) {
            auto stats = ir->getGateStatistics();
            std::cout << "\nGate Statistics:" << std::endl;
            std::cout << "===============" << std::endl;
            for (const auto& [gate, count] : stats) {
                std::cout << gate << ": " << count << std::endl;
            }
            std::cout << "===============" << std::endl;
        }

        // Convert to requested format and write to file
        if (!output_file.empty()) {
            std::ofstream out_file(output_file);
            if (!out_file.is_open()) {
                throw std::runtime_error("Could not open output file: " + output_file);
            }

            OutputFormat format = stringToFormat(format_str);
            std::string output;

            switch (format) {
                case OutputFormat::IR:
                    output = ir->toString();
                    break;
                case OutputFormat::QASM:
                    output = ir->toQASM();
                    break;
                case OutputFormat::JSON:
                    output = ir->toJSON();
                    break;
                case OutputFormat::DOT:
                    output = ir->toDOT();
                    break;
                case OutputFormat::CIRCUIT:
                    output = ir->toCircuitDiagram();
                    break;
            }

            out_file << output;
            if (verbose) {
                std::cout << "Output written to: " << output_file << " (format: " << format_str << ")" << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

} // namespace qasm 