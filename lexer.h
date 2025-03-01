#ifndef lexer
#define lexer
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexerDef.h"
#include "symbolTable.h"

extern char buffer[BUFFER_SIZE];
extern Table *symbol_table;
extern int lexemeBegin;
extern int retraction;
extern int buffer_ptr;
extern int lineNo;
extern int state;

void retract(int num);
char getChar(FILE* fp);
void initLexer();
char* getName();
int getLen();
token getNextToken(FILE* fp);
void printTokens(FILE *testfile);
void populateLookupTable();
void removeComments(char* testcaseFile, char* cleanFile);
#endif