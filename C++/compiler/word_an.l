#include "sytanx_an.tab.h"

%%

[0-9]+                  { yylval = atoi(yytext); return NUMBER; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"for"                   { return FOR; }
"while"                 { return WHILE; }
"int"                   { return INT; }
"float"                 { return FLOAT; }
"void"                  { return VOID; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return TIMES; }
"/"                     { return DIVIDE; }
"="                     { return ASSIGN; }
"=="                    { return EQ; }
"!="                    { return NEQ; }
"<"                     { return LT; }
"<="                    { return LEQ; }
">"                     { return GT; }
">="                    { return GEQ; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
";"                     { return SEMICOLON; }
[ \t]                   { /* 忽略空格和制表符 */ }
\n                      { return EOL; }
"//".*                  { /* 忽略单行注释 */ }
"/*"[^*]*"*"+[^/]*"/"   { /* 忽略多行注释 */ }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval = strdup(yytext); return IDENTIFIER; }
.                       { printf("Unexpected character: %s\n", yytext); }

%%
