%{
/**
 * @file lexer.ll
 * @brief Flex lexer definition for OpenQASM.
 * 
 * This file defines the lexical analyzer for OpenQASM source code.
 * It uses Flex to generate a scanner that tokenizes the input source code.
 */

#include "OpenQASMCompiler/parser.hpp"
#include "parser.yy.hh"
#include <string>
#include <cstdlib>

// Forward declarations
namespace qasm {
    class Parser::Impl;
}

/**
 * @brief Converts a string to a double value.
 * 
 * @param str The string to convert.
 * @return double The converted value.
 * @throw std::invalid_argument If the string cannot be converted to a double.
 */
double str_to_double(const char* str) {
    char* end;
    double val = strtod(str, &end);
    if (*end != '\0') {
        throw std::invalid_argument("Invalid floating-point number");
    }
    return val;
}

%}

%option noyywrap    ///< Don't use yywrap
%option nounput     ///< Don't use unput
%option noinput     ///< Don't use input
%option c++         ///< Generate C++ code

DIGIT       [0-9]           ///< Single digit
INTEGER     {DIGIT}+        ///< One or more digits
FLOAT       {DIGIT}+"."{DIGIT}*  ///< Floating-point number
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*  ///< Variable or gate name

%%

/**
 * @brief Keywords and symbols
 */
"qreg"      { return QREG; }      ///< Quantum register declaration
"creg"      { return CREG; }      ///< Classical register declaration
"gate"      { return GATE; }      ///< Gate declaration
"measure"   { return MEASURE; }   ///< Measurement operation
"h"         { return H; }         ///< Hadamard gate
"x"         { return X; }         ///< Pauli-X gate
"y"         { return Y; }         ///< Pauli-Y gate
"z"         { return Z; }         ///< Pauli-Z gate
"rx"        { return RX; }        ///< Rotation around X axis
"ry"        { return RY; }        ///< Rotation around Y axis
"rz"        { return RZ; }        ///< Rotation around Z axis
"cnot"      { return CNOT; }      ///< Controlled-NOT gate
"cz"        { return CZ; }        ///< Controlled-Z gate
"("         { return LPAREN; }    ///< Left parenthesis
")"         { return RPAREN; }    ///< Right parenthesis
"{"         { return LBRACE; }    ///< Left brace
"}"         { return RBRACE; }    ///< Right brace
","         { return COMMA; }      ///< Comma
";"         { return SEMICOLON; } ///< Semicolon
"->"        { return '->'; }      ///< Measurement arrow
"["         { return '['; }       ///< Left bracket
"]"         { return ']'; }       ///< Right bracket

/**
 * @brief Integer literals
 */
{INTEGER}   {
    yylval.ival = atoi(yytext);
    return INTEGER;
}

/**
 * @brief Floating-point literals
 */
{FLOAT}     {
    yylval.dval = str_to_double(yytext);
    return FLOAT;
}

/**
 * @brief Identifiers
 */
{IDENTIFIER} {
    yylval.sval = new std::string(yytext);
    return IDENTIFIER;
}

/**
 * @brief Whitespace
 */
[ \t\n\r]+  /* Skip whitespace */

/**
 * @brief Invalid characters
 */
.           {
    // Report error for invalid characters
    return -1;
}

%% 