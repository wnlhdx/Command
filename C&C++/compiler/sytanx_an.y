%{  
#include <stdio.h>  
#include "word_an.l"  
  
void yyerror(const char *s);  
%}  
  
%token NUMBER  
%token PLUS MINUS TIMES DIVIDE  
%token LPAREN RPAREN  
%token EOL  
  
%%  
/* 定义语法规则 */  
calclist: /* 空 */  
        | calclist line  
        ;  
  
line:     expression EOL { printf("= %d\n", $1); }  
        ;  
  
expression: term {   
 
$$
 = $1; }  
          | expression PLUS term {   
 
$$
 = $1 + $3; }  
          | expression MINUS term {   
 
$$
 = $1 - $3; }  
          ;  
  
term:     factor {   
 
$$
 = $1; }  
        | term TIMES factor {   
 
$$
 = $1 * $3; }  
        | term DIVIDE factor { if ($3)   
 
$$
 = $1 / $3; else { printf("Error: Division by zero\n"); YYERROR; } }  
        ;  
  
factor:   NUMBER {   
 
$$
 = $1; }  
        | LPAREN expression RPAREN {   
 
$$
 = $2; }  
        ;  
%%  
  
void yyerror(const char *s) {  
    printf("Error: %s\n", s);  