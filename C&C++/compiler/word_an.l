%{  
#include "sytanx_an.tab.h"  
%}  
  
%%  
[0-9]+  { yylval = atoi(yytext); return NUMBER; }  
"+"     { return PLUS; }  
"-"     { return MINUS; }  
"*"     { return TIMES; }  
"/"     { return DIVIDE; }  
"("     { return LPAREN; }  
")"     { return RPAREN; }  
[ \t]   { /* 忽略空格和制表符 */ }  
\n      { return EOL; }  
.       { printf("Unexpected character: %s\n", yytext); }  
%%