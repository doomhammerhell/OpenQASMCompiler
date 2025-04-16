%{
/**
 * @file parser.yy
 * @brief Bison parser definition for OpenQASM.
 * 
 * This file defines the grammar and semantic actions for parsing OpenQASM source code.
 * It uses Bison to generate a parser that constructs an Abstract Syntax Tree (AST)
 * from the input source code.
 */

#include "OpenQASMCompiler/parser.hpp"
#include "OpenQASMCompiler/ast.hpp"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace qasm {
    class Parser::Impl;
}

/**
 * @brief Implementation class for the Parser.
 * 
 * This class contains the internal state and functionality needed by the parser.
 * It is hidden from the public interface using the PIMPL pattern.
 */
class qasm::Parser::Impl {
public:
    std::unique_ptr<ProgramNode> program;      ///< The root of the AST
    std::string error_message;                 ///< The last error message
    std::vector<std::unique_ptr<ASTNode>> statements;  ///< Current list of statements
    
    /**
     * @brief Adds a statement to the current list of statements.
     * 
     * @param stmt The statement to add.
     */
    void addStatement(std::unique_ptr<ASTNode> stmt) {
        statements.push_back(std::move(stmt));
    }
    
    /**
     * @brief Sets the error message.
     * 
     * @param msg The error message to set.
     */
    void setError(const std::string& msg) {
        error_message = msg;
    }
};

// Define the union type for parser values
%union {
    int ival;                                  ///< Integer values
    double dval;                               ///< Floating-point values
    std::string* sval;                         ///< String values
    std::vector<size_t>* qubits;               ///< Qubit indices
    std::vector<std::string>* strings;         ///< String lists
    std::unique_ptr<qasm::ASTNode>* node;      ///< AST nodes
    std::unique_ptr<qasm::ProgramNode>* program;  ///< Program nodes
}

// Define token types
%token <sval> IDENTIFIER    ///< Variable and gate names
%token <ival> INTEGER       ///< Integer literals
%token <dval> FLOAT        ///< Floating-point literals
%token QREG CREG GATE MEASURE  ///< Keywords
%token H X Y Z RX RY RZ CNOT CZ  ///< Gate names
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON  ///< Punctuation

// Define operator precedence
%left '+'  ///< Addition operator
%left '*'  ///< Multiplication operator

// Define the type of the result
%type <node> statement gate_declaration gate_application measurement
%type <qubits> qubit_list
%type <strings> parameter_list qubit_declaration_list
%type <program> program

%%

/**
 * @brief Program grammar rule.
 * 
 * A program consists of a sequence of statements.
 */
program
    : statements { 
        $$ = std::make_unique<qasm::ProgramNode>();
        for (auto& stmt : $1) {
            $$->addStatement(std::move(stmt));
        }
    }
    ;

/**
 * @brief Statements grammar rule.
 * 
 * A sequence of statements, either a single statement or multiple statements.
 */
statements
    : statement { $$ = std::vector<std::unique_ptr<qasm::ASTNode>>(); $$.push_back(std::move($1)); }
    | statements statement { $1.push_back(std::move($2)); $$ = std::move($1); }
    ;

/**
 * @brief Statement grammar rule.
 * 
 * A statement can be a gate declaration, gate application, measurement,
 * or register declaration.
 */
statement
    : gate_declaration { $$ = std::move($1); }
    | gate_application { $$ = std::move($1); }
    | measurement { $$ = std::move($1); }
    | QREG IDENTIFIER '[' INTEGER ']' SEMICOLON {
        $$ = std::make_unique<qasm::QubitDeclarationNode>($4);
    }
    | CREG IDENTIFIER '[' INTEGER ']' SEMICOLON {
        $$ = std::make_unique<qasm::ClassicalBitDeclarationNode>($4);
    }
    ;

/**
 * @brief Gate declaration grammar rule.
 * 
 * Defines a new quantum gate with parameters and qubits.
 */
gate_declaration
    : GATE IDENTIFIER parameter_list qubit_declaration_list LBRACE statements RBRACE {
        $$ = std::make_unique<qasm::GateDeclarationNode>(*$2, *$3, *$4, std::move($6));
        delete $2; delete $3; delete $4;
    }
    ;

/**
 * @brief Parameter list grammar rule.
 * 
 * A list of parameters for a gate declaration.
 */
parameter_list
    : LPAREN RPAREN { $$ = new std::vector<std::string>(); }
    | LPAREN IDENTIFIER { $$ = new std::vector<std::string>(); $$->push_back(*$2); delete $2; }
    | parameter_list COMMA IDENTIFIER { $1->push_back(*$3); $$ = $1; delete $3; }
    ;

/**
 * @brief Qubit declaration list grammar rule.
 * 
 * A list of qubit identifiers for a gate declaration.
 */
qubit_declaration_list
    : IDENTIFIER { $$ = new std::vector<std::string>(); $$->push_back(*$1); delete $1; }
    | qubit_declaration_list IDENTIFIER { $1->push_back(*$2); $$ = $1; delete $2; }
    ;

/**
 * @brief Gate application grammar rule.
 * 
 * Applies a quantum gate to a set of qubits.
 */
gate_application
    : IDENTIFIER qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::CustomGateNode>(*$1, std::vector<std::vector<std::complex<double>>>(), *$2);
        delete $1; delete $2;
    }
    | H qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::H, *$2);
        delete $2;
    }
    | X qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::X, *$2);
        delete $2;
    }
    | Y qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::Y, *$2);
        delete $2;
    }
    | Z qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::Z, *$2);
        delete $2;
    }
    | RX LPAREN FLOAT RPAREN qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::ParameterizedGateNode>(qasm::GateType::RX, *$5, $3);
        delete $5;
    }
    | RY LPAREN FLOAT RPAREN qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::ParameterizedGateNode>(qasm::GateType::RY, *$5, $3);
        delete $5;
    }
    | RZ LPAREN FLOAT RPAREN qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::ParameterizedGateNode>(qasm::GateType::RZ, *$5, $3);
        delete $5;
    }
    | CNOT qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::CNOT, *$2);
        delete $2;
    }
    | CZ qubit_list SEMICOLON {
        $$ = std::make_unique<qasm::GateNode>(qasm::GateType::CZ, *$2);
        delete $2;
    }
    ;

/**
 * @brief Qubit list grammar rule.
 * 
 * A list of qubit indices for gate application.
 */
qubit_list
    : IDENTIFIER { $$ = new std::vector<size_t>(); $$->push_back(0); }
    | IDENTIFIER '[' INTEGER ']' { $$ = new std::vector<size_t>(); $$->push_back($3); }
    | qubit_list COMMA IDENTIFIER { $1->push_back(0); $$ = $1; }
    | qubit_list COMMA IDENTIFIER '[' INTEGER ']' { $1->push_back($5); $$ = $1; }
    ;

/**
 * @brief Measurement grammar rule.
 * 
 * Measures a qubit and stores the result in a classical bit.
 */
measurement
    : MEASURE IDENTIFIER '[' INTEGER ']' '->' IDENTIFIER '[' INTEGER ']' SEMICOLON {
        $$ = std::make_unique<qasm::MeasureNode>($4, $8);
    }
    ;

%%

/**
 * @brief Error handling function for the parser.
 * 
 * @param msg The error message.
 */
void yyerror(const char* msg) {
    // Store the error message in the parser implementation
    // This will be handled by the Parser class
}

// Lexer interface
int yylex();
void yylex_destroy(); 