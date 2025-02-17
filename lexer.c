#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char buffer[BUFFER_SIZE];
extern Table *symbol_table;
extern int lexemeBegin;
extern int retraction;
extern int forward;
extern int lineNo;
extern int state;

void removeComments(char *testCaseFile, char *cleanFile) {
    FILE *testFile = fopen(testCaseFile, "r");
    FILE *clean = fopen(cleanFile, "w");
    int flag = 0;
    char ch;
    while ((ch == getc(testFile)) != EOF) {
        if (ch == '%') {
            flag = 1;
        }

        if (flag == 0) {
            fputc(ch, clean);
        }

        if (ch == '\n') {
            flag = 0;
        }
    }
    fclose(testFile);
    fclose(clean);
}

void retract(int num) {}