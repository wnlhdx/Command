%{
#include "parser.tab.h"
%}

%%
"int"           { return INT; }
[0-9]+          { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*   { yylval.sval = strdup(yytext); return ID; }
"="             { return ASSIGN; }
"+"             { return PLUS; }
";"             { return SEMICOLON; }
[ \t\n]+        ;   // 忽略空白
.               { printf("Unexpected character: %s\n", yytext); }
%%
