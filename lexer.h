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
void retract(int num);
char getChar(FILE* fp);
void initLexer();
char* getName();
int getLen();
token getNextToken(FILE* fp);
void printTokens(char* testfile);
void populateLookupTable();
void removeComments(char* testcaseFile, char* cleanFile);
#endif