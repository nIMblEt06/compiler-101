#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[BUFFER_SIZE];
Table *symbol_table;
int lexemeBegin;
int retraction;
int buffer_ptr;
int lineNo;
int state;

void removeComments(char *testCaseFile, char *cleanFile) {
    FILE *testFile = fopen(testCaseFile, "r");
    FILE *clean = fopen(cleanFile, "w");
    if (!testFile || !clean) {
        printf("Error: Could not open files for comment removal\n");
        return;
    }
    int flag = 0;
    int ch;
    while ((ch = fgetc(testFile)) != EOF) {
        if (ch == '%') {
            flag = 1;
        } else if (ch == '\n') {
            flag = 0;
            fputc(ch, clean);
        } else if (flag == 0) {
            fputc(ch, clean);
        }
    }
    fclose(testFile);
    fclose(clean);
}

void retract(int num) {
    buffer_ptr -= num;
    if (buffer_ptr < 0) {
        buffer_ptr += BUFFER_SIZE;
    }
    retraction += num;
}

void initLexer() {
    buffer_ptr = 0;
    lexemeBegin = 0;
    lineNo = 1;  // Start from line 1
    retraction = 0;
    state = 0;
    
    // Initialize buffer
    memset(buffer, 0, BUFFER_SIZE);
    
    // Initialize symbol table
    symbol_table = createTable();
    populateLookupTable();
}

void populateLookupTable() {
    insert(symbol_table, "with", "TK_WITH");
    insert(symbol_table, "parameters", "TK_PARAMETERS");
    insert(symbol_table, "end", "TK_END");
    insert(symbol_table, "while", "TK_WHILE");
    insert(symbol_table, "union", "TK_UNION");
    insert(symbol_table, "endunion", "TK_ENDUNION");
    insert(symbol_table, "definetype", "TK_DEFINETYPE");
    insert(symbol_table, "as", "TK_AS");
    insert(symbol_table, "type", "TK_TYPE");
    insert(symbol_table, "_main", "TK_MAIN");
    insert(symbol_table, "global", "TK_GLOBAL");
    insert(symbol_table, "parameter", "TK_PARAMETER");
    insert(symbol_table, "list", "TK_LIST");
    insert(symbol_table, "input", "TK_INPUT");
    insert(symbol_table, "output", "TK_OUTPUT");
    insert(symbol_table, "int", "TK_INT");
    insert(symbol_table, "real", "TK_REAL");
    insert(symbol_table, "endwhile", "TK_ENDWHILE");
    insert(symbol_table, "if", "TK_IF");
    insert(symbol_table, "then", "TK_THEN");
    insert(symbol_table, "endif", "TK_ENDIF");
    insert(symbol_table, "read", "TK_READ");
    insert(symbol_table, "write", "TK_WRITE");
    insert(symbol_table, "return", "TK_RETURN");
    insert(symbol_table, "call", "TK_CALL");
    insert(symbol_table, "record", "TK_RECORD");
    insert(symbol_table, "endrecord", "TK_ENDRECORD");
    insert(symbol_table, "else", "TK_ELSE");
}
int getLen() {
    if (lexemeBegin <= buffer_ptr) {
        return buffer_ptr - lexemeBegin;
    } else {  // if lexeme starts in the 2nd part of twin buffer
        return buffer_ptr + BUFFER_SIZE - lexemeBegin;
    }
}

char* getName() {
    int len = getLen();
    char* name = (char*)malloc(MAX_LEXEME_LEN * sizeof(char));
    if (!name) {
        printf("Error: Memory allocation failed in getName()\n");
        return NULL;
    }
    int i = 0;
    while (i < len && i < MAX_LEXEME_LEN - 1) {
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
        ch = buffer[buffer_ptr];
        buffer_ptr = (buffer_ptr + 1) % BUFFER_SIZE;
        return ch;
    } else {
        ch = fgetc(fp);
        if (ch == '\n') {
            lineNo++;
        }
        buffer[buffer_ptr] = ch;
        buffer_ptr = (buffer_ptr + 1) % BUFFER_SIZE;
        return ch;
    }
}

char* createTokenString(const char* str) {
    char* token = strdup(str);
    return token;
}

token getNextToken(FILE* fp) {
    if (!symbol_table) {
        initLexer();
    }

    token tk = {NULL, NULL, lineNo};
    char c;
    state = 0;
    lexemeBegin = buffer_ptr;
    int starting_line = lineNo; // Store starting line for this token

    while(1) {
        switch(state) {
            case 0:
                c = getChar(fp);
                if (c == EOF) {
                    tk.name = createTokenString("EOF");
                    tk.lexeme_value = createTokenString("EOF");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '\n') {
                    // Start fresh for next token after newline - don't return a token
                    starting_line = lineNo; // Update starting line
                    lexemeBegin = buffer_ptr; // Reset lexeme beginning
                    continue;
                } else if (c == '+') {
                    tk.name = createTokenString("TK_PLUS");
                    tk.lexeme_value = createTokenString("+");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '-') {
                    tk.name = createTokenString("TK_MINUS");
                    tk.lexeme_value = createTokenString("-");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '*') {
                    tk.name = createTokenString("TK_MUL");
                    tk.lexeme_value = createTokenString("*");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '/') {
                    tk.name = createTokenString("TK_DIV");
                    tk.lexeme_value = createTokenString("/");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '(') {
                    tk.name = createTokenString("TK_OP");
                    tk.lexeme_value = createTokenString("(");
                    tk.line = starting_line;
                    return tk;
                } else if (c == ')') {
                    tk.name = createTokenString("TK_CL");
                    tk.lexeme_value = createTokenString(")");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '[') {
                    tk.name = createTokenString("TK_SQL");
                    tk.lexeme_value = createTokenString("[");
                    tk.line = starting_line;
                    return tk;
                } else if (c == ']') {
                    tk.name = createTokenString("TK_SQR");
                    tk.lexeme_value = createTokenString("]");
                    tk.line = starting_line;
                    return tk;
                } else if (c == ',') {
                    tk.name = createTokenString("TK_COMMA");
                    tk.lexeme_value = createTokenString(",");
                    tk.line = starting_line;
                    return tk;
                } else if (c == ':') {
                    tk.name = createTokenString("TK_COLON");
                    tk.lexeme_value = createTokenString(":");
                    tk.line = starting_line;
                    return tk;
                } else if (c == ';') {
                    tk.name = createTokenString("TK_SEM");
                    tk.lexeme_value = createTokenString(";");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '.') {
                    tk.name = createTokenString("TK_DOT");
                    tk.lexeme_value = createTokenString(".");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '%') {
                    state = 33;  // Comment state
                    continue;
                } else if (c == '<') {
                    state = 1;
                    continue;
                } else if (c == '>') {
                    state = 5;
                    continue;
                } else if (c == '=') {
                    state = 8;
                    continue;
                } else if (c == '!') {
                    state = 10;
                    continue;
                } else if (c == '~') {
                    tk.name = createTokenString("TK_NOT");
                    tk.lexeme_value = createTokenString("~");
                    tk.line = starting_line;
                    return tk;
                } else if (c == '&') {
                    state = 12;
                    continue;
                } else if (c == '|') {
                    state = 15;
                    continue;
                } else if (c == ' ' || c == '\t') {
                    continue;
                } else if (isalpha(c) || c == '_' || c == '#') {
                    retract(1);
                    state = 18;  // Identifier/keyword state
                    continue;
                } else if (isdigit(c)) {
                    retract(1);
                    state = 40;  // Number state
                    continue;
                } else {
                    // Handle unknown characters
                    char error_msg[100];
                    snprintf(error_msg, sizeof(error_msg), "Unknown character: %c", c);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString(error_msg);
                    tk.line = starting_line;
                    return tk;
                }
                
            case 1: // Processing '<'
                c = getChar(fp);
                if (c == '-') {
                    state = 2;
                    continue;
                } else if (c == '=') {
                    tk.name = createTokenString("TK_LE");
                    tk.lexeme_value = createTokenString("<=");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("TK_LT");
                    tk.lexeme_value = createTokenString("<");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 2: // Processing '<-'
                c = getChar(fp);
                if (c == '-') {
                    state = 3;
                    continue;
                } else {
                    retract(2);
                    tk.name = createTokenString("TK_LT");
                    tk.lexeme_value = createTokenString("<");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 3: // Processing '<--'
                c = getChar(fp);
                if (c == '-') {
                    tk.name = createTokenString("TK_ASSIGNOP");
                    tk.lexeme_value = createTokenString("<---");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("TK_ASSIGNOP");
                    tk.lexeme_value = createTokenString("<--");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 5: // Processing '>'
                c = getChar(fp);
                if (c == '=') {
                    tk.name = createTokenString("TK_GE");
                    tk.lexeme_value = createTokenString(">=");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("TK_GT");
                    tk.lexeme_value = createTokenString(">");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 8: // Processing '='
                c = getChar(fp);
                if (c == '=') {
                    tk.name = createTokenString("TK_EQ");
                    tk.lexeme_value = createTokenString("==");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    // Single '=' is an error in this language
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: =");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 10: // Processing '!'
                c = getChar(fp);
                if (c == '=') {
                    tk.name = createTokenString("TK_NE");
                    tk.lexeme_value = createTokenString("!=");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    // Single '!' is an error in this language
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: !");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 12: // Processing '&'
                c = getChar(fp);
                if (c == '&') {
                    state = 13;
                    continue;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: &");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 13: // Processing '&&'
                c = getChar(fp);
                if (c == '&') {
                    tk.name = createTokenString("TK_AND");
                    tk.lexeme_value = createTokenString("&&&");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: &&");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 15: // Processing '|'
                c = getChar(fp);
                if (c == '|') {
                    state = 16;
                    continue;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: |");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 16: // Processing '||'
                c = getChar(fp);
                if (c == '|') {
                    tk.name = createTokenString("TK_OR");
                    tk.lexeme_value = createTokenString("|||");
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid operator: ||");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 18: // Processing identifiers and keywords
                c = getChar(fp);
                if (c == '_') {
                    // Function identifier starts with '_'
                    state = 20;
                    continue;
                } else if (c == '#') {
                    // Record/Union identifier starts with '#'
                    state = 25;
                    continue;
                } else if (isalpha(c)) {
                    // Regular identifier or keyword
                    state = 30;
                    continue;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid identifier start");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 20: // Processing function identifier (starts with '_')
                c = getChar(fp);
                if (isalnum(c)) {
                    char lexeme[MAX_LEXEME_LEN];
                    int i = 0;
                    lexeme[i++] = '_';
                    lexeme[i++] = c;
                    
                    while (i < MAX_LEXEME_LEN - 1) {
                        c = getChar(fp);
                        if (!isalnum(c)) {
                            retract(1);
                            break;
                        }
                        lexeme[i++] = c;
                    }
                    lexeme[i] = '\0';
                    
                    // Check if it's a keyword
                    char* keyword = search(symbol_table, lexeme);
                    if (strcmp(keyword, "\0") != 0) {
                        tk.name = createTokenString(keyword);
                    } else {
                        tk.name = createTokenString("TK_FUNID");
                    }
                    tk.lexeme_value = createTokenString(lexeme);
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid function identifier");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 25: // Processing record/union identifier (starts with '#')
                c = getChar(fp);
                if (isalnum(c)) {
                    char lexeme[MAX_LEXEME_LEN];
                    int i = 0;
                    lexeme[i++] = '#';
                    lexeme[i++] = c;
                    
                    while (i < MAX_LEXEME_LEN - 1) {
                        c = getChar(fp);
                        if (!isalnum(c)) {
                            retract(1);
                            break;
                        }
                        lexeme[i++] = c;
                    }
                    lexeme[i] = '\0';
                    
                    tk.name = createTokenString("TK_RUID");
                    tk.lexeme_value = createTokenString(lexeme);
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid record/union identifier");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 30: // Processing regular identifier or keyword
            {
                char lexeme[MAX_LEXEME_LEN];
                int i = 0;
                lexeme[i++] = c;
                
                while (i < MAX_LEXEME_LEN - 1) {
                    c = getChar(fp);
                    if (!isalnum(c) && c != '_') {
                        retract(1);
                        break;
                    }
                    lexeme[i++] = c;
                }
                lexeme[i] = '\0';
                
                // Check if it's a keyword
                char* keyword = search(symbol_table, lexeme);
                if (strcmp(keyword, "\0") != 0) {
                    tk.name = createTokenString(keyword);
                } else {
                    // Check if it's a field identifier (starts with lowercase)
                    if (islower(lexeme[0])) {
                        tk.name = createTokenString("TK_FIELDID");
                    } else {
                        tk.name = createTokenString("TK_ID");
                    }
                }
                tk.lexeme_value = createTokenString(lexeme);
                tk.line = starting_line;
                return tk;
            }
                
            case 33: // Comment state
                c = getChar(fp);
                if (c == '\n') {
                    state = 0;  // Return to initial state
                    continue;
                } else if (c == EOF) {
                    tk.name = createTokenString("EOF");
                    tk.lexeme_value = createTokenString("EOF");
                    tk.line = starting_line;
                    return tk;
                }
                state = 33;
                break;
                
            case 40: // Number state
                c = getChar(fp);
                if (isdigit(c)) {
                    char num[MAX_LEXEME_LEN];
                    int i = 0;
                    num[i++] = c;
                    
                    while (i < MAX_LEXEME_LEN - 1) {
                        c = getChar(fp);
                        if (!isdigit(c) && c != '.') {
                            retract(1);
                            break;
                        }
                        num[i++] = c;
                    }
                    num[i] = '\0';
                    
                    if (strchr(num, '.') == NULL) {
                        tk.name = createTokenString("TK_NUM");
                        tk.lexeme_value = createTokenString(num);
                        tk.num = atoi(num);
                    } else {
                        tk.name = createTokenString("TK_RNUM");
                        tk.lexeme_value = createTokenString(num);
                        tk.rnum = atof(num);
                    }
                    tk.line = starting_line;
                    return tk;
                } else {
                    retract(1);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString("Invalid number format");
                    tk.line = starting_line;
                    return tk;
                }
                
            case 60: // Whitespace state - we'll skip whitespace instead of creating tokens
                c = getChar(fp);
                if (c == ' ' || c == '\t') {
                    state = 60;
                } else {
                    retract(1);
                    state = 0;  // Return to initial state
                    continue;
                }
                break;
                
            default:
                // Handle unknown state
                tk.name = createTokenString("ERROR");
                tk.lexeme_value = createTokenString("Unknown lexer state");
                tk.line = starting_line;
                return tk;
        }
    }
}

void printTokens(FILE *testfile) {
    printf("\n%-15s %-20s %-10s\n", "TOKEN", "LEXEME", "LINE NO.");
    printf("------------------------------------------------------\n");

    while (1) {
        token tk = getNextToken(testfile);

        // Stop if end-of-file is reached
        if (strcmp(tk.name, "EOF") == 0) {
            break;
        }
		if(strcmp(tk.name, "SPACE") == 0) continue;
		if(strcmp(tk.name, "TK_NUM") == 0){
			printf("%-15s %-20d %-10d\n", tk.name, tk.num, tk.line);
		}
		else if(strcmp(tk.name, "TK_RNUM") == 0){
			printf("%-15s %-20f %-10d\n", tk.name, tk.rnum, tk.line);
		}
        // Pretty-printing tokens
        else printf("%-15s %-20s %-10d\n", tk.name, tk.lexeme_value, tk.line);
    }
}