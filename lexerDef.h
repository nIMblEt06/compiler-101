#ifndef lexerDef
#define lexerDef
#define BUFFER_SIZE 4096
#define maxlen 65
#include <stdio.h>

#include "symbolTable.h"

typedef struct TokenInfo {
    char *name;
    union {
        char *lexeme_value;
        int num;
        double rnum;
    };
    int line;
} token;

#endif