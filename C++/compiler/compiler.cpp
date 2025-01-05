#include <stdio.h>
#include <stdlib.h>
#include "sytanx_an.tab.h"

extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }

    printf("Starting the compiler...\n");

    yyparse();  // 启动语法分析
    return 0;
}
