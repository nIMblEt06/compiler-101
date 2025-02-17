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

void retract(int num) {
    forward -= num;
    if (forward < 0) {
        forward += BUFFER_SIZE;
    }
    retraction += num;
}

void initLexer() {
    forward = 0;
    lexemeBegin = 0;
    lineNo = 1;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = '\0';
    }
    retraction = 0;
    state = 0;
    symbol_table = createTable();
    populateLookupTable();
}

void populateLookupTable() {
    insert(symbol_table, "TK_ASSIGNOP", "TK_ASSIGNOP");
    insert(symbol_table, "TK_COMMENT", "TK_COMMENT");
    insert(symbol_table, "TK_FIELDID", "TK_FIELDID");
    insert(symbol_table, "TK_ID", "TK_ID");
    insert(symbol_table, "TK_NUM", "TK_NUM");
    insert(symbol_table, "TK_RNUM", "TK_RNUM");
    insert(symbol_table, "TK_FUNID", "TK_FUNID");
    insert(symbol_table, "TK_RUID", "TK_RUID");
    insert(symbol_table, "TK_WITH", "TK_WITH");
    insert(symbol_table, "TK_PARAMETERS", "TK_PARAMETERS");
    insert(symbol_table, "TK_END", "TK_END");
    insert(symbol_table, "TK_WHILE", "TK_WHILE");
    insert(symbol_table, "TK_UNION", "TK_UNION");
    insert(symbol_table, "TK_ENDUNION", "TK_ENDUNION");
    insert(symbol_table, "TK_DEFINETYPE", "TK_DEFINETYPE");
    insert(symbol_table, "TK_AS", "TK_AS");
    insert(symbol_table, "TK_TYPE", "TK_TYPE");
    insert(symbol_table, "TK_MAIN", "TK_MAIN");
    insert(symbol_table, "TK_GLOBAL", "TK_GLOBAL");
    insert(symbol_table, "TK_PARAMETER", "TK_PARAMETER");
    insert(symbol_table, "TK_LIST", "TK_LIST");
    insert(symbol_table, "TK_SQL", "TK_SQL");
    insert(symbol_table, "TK_SQR", "TK_SQR");
    insert(symbol_table, "TK_INPUT", "TK_INPUT");
    insert(symbol_table, "TK_OUTPUT", "TK_OUTPUT");
    insert(symbol_table, "TK_INT", "TK_INT");
    insert(symbol_table, "TK_REAL", "TK_REAL");
    insert(symbol_table, "TK_COMMA", "TK_COMMA");
    insert(symbol_table, "TK_SEM", "TK_SEM");
    insert(symbol_table, "TK_COLON", "TK_COLON");
    insert(symbol_table, "TK_DOT", "TK_DOT");
    insert(symbol_table, "TK_ENDWHILE", "TK_ENDWHILE");
    insert(symbol_table, "TK_OP", "TK_OP");
    insert(symbol_table, "TK_CL", "TK_CL");
    insert(symbol_table, "TK_IF", "TK_IF");
    insert(symbol_table, "TK_THEN", "TK_THEN");
    insert(symbol_table, "TK_ENDIF", "TK_ENDIF");
    insert(symbol_table, "TK_READ", "TK_READ");
    insert(symbol_table, "TK_WRITE", "TK_WRITE");
    insert(symbol_table, "TK_RETURN", "TK_RETURN");
    insert(symbol_table, "TK_PLUS", "TK_PLUS");
    insert(symbol_table, "TK_MINUS", "TK_MINUS");
    insert(symbol_table, "TK_MUL", "TK_MUL");
    insert(symbol_table, "TK_DIV", "TK_DIV");
    insert(symbol_table, "TK_CALL", "TK_CALL");
    insert(symbol_table, "TK_RECORD", "TK_RECORD");
    insert(symbol_table, "TK_ENDRECORD", "TK_ENDRECORD");
    insert(symbol_table, "TK_ELSE", "TK_ELSE");
    insert(symbol_table, "TK_AND", "TK_AND");
    insert(symbol_table, "TK_OR", "TK_OR");
    insert(symbol_table, "TK_NOT", "TK_NOT");
    insert(symbol_table, "TK_LT", "TK_LT");
    insert(symbol_table, "TK_LE", "TK_LE");
    insert(symbol_table, "TK_EQ", "TK_EQ");
    insert(symbol_table, "TK_GT", "TK_GT");
    insert(symbol_table, "TK_GE", "TK_GE");
    insert(symbol_table, "TK_NE", "TK_NE");
}

int getLen() {
    if (lexemeBegin <= forward) {
        return forward - lexemeBegin;
    } else {  // if lexeme starts in the 2nd part of twin buffer
        return forward + BUFFER_SIZE - lexemeBegin;
    }
}

char* getName() {
    int len = getLen();
    char* name = (char*)malloc(maxlen);
    int i = 0;
    while (i < len) {
        name[i] = buffer[(lexemeBegin + i) % BUFFER_SIZE];
        i++;
    }
    name[i] = '\0';
    return name;
}

char getChar(FILE* fp) {
    char ch;
    if (retraction > 0) {
        retraction--;
        ch = buffer[forward];
        forward = (forward + 1) % BUFFER_SIZE;
        return ch;
    } else {
        ch = fgetc(fp);
        if (ch == '\n') {
            lineNo++;
        }
        buffer[forward] = ch;
        forward = (forward + 1) % BUFFER_SIZE;
        return ch;
    }
}

token getNextToken(FILE* fp) {
    token tk;
    state = 0;
    tk.line = lineNo;


}

void printToken(char *testfile) {
    
}