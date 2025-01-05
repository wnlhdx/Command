%{
#include <stdio.h>
#include <stdlib.h>
#include "word_an.l"

void yyerror(const char *s);
%}

%token NUMBER IDENTIFIER IF ELSE FOR WHILE INT FLOAT VOID
%token PLUS MINUS TIMES DIVIDE ASSIGN EQ NEQ LT LEQ GT GEQ
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON EOL

%%

program:
    | program function
    | program statement
    ;

function:
    type IDENTIFIER LPAREN args RPAREN compound_statement
    ;

args:
      | args type IDENTIFIER
      ;

type:
    INT
    | FLOAT
    | VOID
    ;

compound_statement:
    LBRACE statement_list RBRACE
    ;

statement_list:
      | statement_list statement
      ;

statement:
    expression_statement
    | declaration_statement
    | if_statement
    ;

expression_statement:
    expression SEMICOLON
    ;

declaration_statement:
    type IDENTIFIER SEMICOLON
    ;

if_statement:
    IF LPAREN expression RPAREN statement ELSE statement
    ;

expression:
    term
    | expression PLUS term { $$ = $1 + $3; }
    | expression MINUS term { $$ = $1 - $3; }
    ;

term:
    factor
    | term TIMES factor { $$ = $1 * $3; }
    | term DIVIDE factor { $$ = $1 / $3; }
    ;

factor:
    NUMBER
    | IDENTIFIER
    | LPAREN expression RPAREN
    ;

%%

void yyerror(const char *s) {
    printf("Error: %s\n", s);
}
