%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
using namespace std;

extern int yylex();
void yyerror(const char *s) { printf("Error: %s\n", s); }

unordered_map<string, int> symbol_table;
%}

%union {
    int ival;
    char* sval;
}

%token <ival> NUMBER
%token <sval> ID
%token INT ASSIGN SEMICOLON PLUS

%type <ival> expression

%%

program:
      | program statement
      ;

statement:
    INT ID ASSIGN expression SEMICOLON {
        symbol_table[$2] = $4;
        printf("Compiled: int %s = %d\n", $2, $4);
    }
    ;

expression:
      NUMBER                 { $$ = $1; }
    | expression PLUS NUMBER { $$ = $1 + $3; }
    ;

%%
