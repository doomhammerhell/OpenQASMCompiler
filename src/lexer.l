%{
#include "parser.hpp"
#include <string>

#define YY_DECL int yylex()

int line_num = 1;
%}

%option noyywrap

%%

"OPENQASM"      { return OPENQASM; }
"include"       { return INCLUDE; }
"qreg"          { return QREG; }
"creg"          { return CREG; }
"measure"       { return MEASURE; }
"->"            { return ARROW; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMICOLON; }
","             { return COMMA; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"=="            { return EQ; }
"="             { return ASSIGN; }

[0-9]+\.[0-9]+ { yylval.real = atof(yytext); return REAL; }
[0-9]+         { yylval.integer = atoi(yytext); return INTEGER; }
[a-zA-Z][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return ID; }
\"[^\"]*\"     { yylval.string = strdup(yytext); return STRING; }

[ \t]          ; // ignore whitespace
\n             { line_num++; }
\/\/[^\n]*     ; // ignore comments

.              { printf("Unknown character: %s\n", yytext); }

%% 