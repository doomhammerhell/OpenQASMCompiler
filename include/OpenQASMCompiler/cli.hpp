#ifndef OPENQASM_CLI_HPP
#define OPENQASM_CLI_HPP

/**
 * @file cli.hpp
 * @brief Command-line interface for the OpenQASM compiler
 * 
 * This file defines the interface for the command-line tool that compiles
 * OpenQASM files into various output formats and performs optimizations.
 */

namespace qasm {

/**
 * @brief Parse command line arguments and execute the requested operations
 * 
 * This function handles the command-line interface for the OpenQASM compiler.
 * It supports various operations including:
 * - Parsing QASM files
 * - Generating intermediate representation
 * - Optimizing quantum circuits
 * - Converting to different output formats
 * - Validating circuits
 * - Showing gate statistics
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, non-zero on error
 * 
 * @throws std::runtime_error if file operations fail
 * @throws std::invalid_argument if invalid arguments are provided
 */
int parseCommandLine(int argc, char** argv);

} // namespace qasm

#endif // OPENQASM_CLI_HPP 