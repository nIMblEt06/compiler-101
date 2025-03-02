#ifndef LEXER_H
#define LEXER_H
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbolTable.h"

#define BUFFER_SIZE 4096
#define MAX_LEXEME_LEN 100  // Maximum length of a lexeme/token

typedef struct {
    char* name;           // Token name (e.g., "TK_NUM", "TK_ID")
    char* lexeme_value;   // Actual lexeme value
    int line;            // Line number where token appears
    union {
        int num;         // For integer tokens
        float rnum;      // For real number tokens
    };
} token;

extern char buffer[BUFFER_SIZE];
extern Table *symbol_table;
extern int lexemeBegin;
extern int retraction;
extern int buffer_ptr;
extern int lineNo;
extern int state;

// Function declarations
void initLexer();
token getNextToken(FILE* fp);
char getChar(FILE* fp);
void retract(int n);
int getLen();
char* getName();

void printTokens(FILE *testfile);
void populateLookupTable();
void removeComments(char* testcaseFile, char* cleanFile);
#endif