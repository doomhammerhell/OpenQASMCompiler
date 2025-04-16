%{
#include <stdio.h>
#include <string>
#include "ast/ast.hpp"

extern int yylex();
extern int line_num;
void yyerror(const char *s) { printf("ERROR (line %d): %s\n", line_num, s); }
%}

%union {
    int integer;
    double real;
    char* string;
    qasm::Node* node;
    qasm::Program* program;
}

%token OPENQASM INCLUDE
%token QREG CREG
%token MEASURE
%token ARROW
%token LBRACKET RBRACKET LBRACE RBRACE LPAREN RPAREN
%token SEMICOLON COMMA
%token PLUS MINUS TIMES DIVIDE
%token EQ ASSIGN
%token <integer> INTEGER
%token <real> REAL
%token <string> ID STRING

%type <program> program
%type <node> statement
%type <node> qreg_decl
%type <node> creg_decl
%type <node> measure_stmt

%%

program
    : OPENQASM REAL SEMICOLON includes statements
        { $$ = new qasm::Program(); }
    ;

includes
    : INCLUDE STRING SEMICOLON
    | includes INCLUDE STRING SEMICOLON
    ;

statements
    : statement
        { /* Add statement to program */ }
    | statements statement
        { /* Add statement to program */ }
    ;

statement
    : qreg_decl
    | creg_decl
    | measure_stmt
    ;

qreg_decl
    : QREG ID LBRACKET INTEGER RBRACKET SEMICOLON
        { $$ = new qasm::QRegDecl($2, $4); }
    ;

creg_decl
    : CREG ID LBRACKET INTEGER RBRACKET SEMICOLON
        { $$ = new qasm::CRegDecl($2, $4); }
    ;

measure_stmt
    : MEASURE ID ARROW ID SEMICOLON
        { /* Create measurement node */ }
    ;

%% 