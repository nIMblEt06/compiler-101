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
    if (!symbol_table) {
        printf("Error: Failed to create symbol table\n");
        exit(1);
    }
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
    if (!token) {
        printf("Error: Memory allocation failed for token string\n");
        exit(1);
    }
    return token;
}

token getNextToken(FILE* fp) {
    if (!symbol_table) {
        initLexer();
    }

    token tk = {NULL, NULL, lineNo};
    char c;
    state = 0;

    while(1) {
        switch(state) {
            case 0:
                c = getChar(fp);
                if (c == EOF) {
                    tk.name = createTokenString("EOF");
                    tk.lexeme_value = createTokenString("EOF");
                    return tk;
                } else if (c == '\n') {
                    lineNo++;
                    continue;  // Skip line breaks
                } else if (c == '+') {
                    tk.name = createTokenString("TK_PLUS");
                    tk.lexeme_value = createTokenString("+");
                    return tk;
                } else if (c == '-') {
                    tk.name = createTokenString("TK_MINUS");
                    tk.lexeme_value = createTokenString("-");
                    return tk;
                } else if (c == '*') {
                    tk.name = createTokenString("TK_MUL");
                    tk.lexeme_value = createTokenString("*");
                    return tk;
                } else if (c == '/') {
                    tk.name = createTokenString("TK_DIV");
                    tk.lexeme_value = createTokenString("/");
                    return tk;
                } else if (c == '(') {
                    tk.name = createTokenString("TK_OP");
                    tk.lexeme_value = createTokenString("(");
                    return tk;
                } else if (c == ')') {
                    tk.name = createTokenString("TK_CL");
                    tk.lexeme_value = createTokenString(")");
                    return tk;
                } else if (c == '[') {
                    tk.name = createTokenString("TK_SQL");
                    tk.lexeme_value = createTokenString("[");
                    return tk;
                } else if (c == ']') {
                    tk.name = createTokenString("TK_SQR");
                    tk.lexeme_value = createTokenString("]");
                    return tk;
                } else if (c == ',') {
                    tk.name = createTokenString("TK_COMMA");
                    tk.lexeme_value = createTokenString(",");
                    return tk;
                } else if (c == ':') {
                    tk.name = createTokenString("TK_COLON");
                    tk.lexeme_value = createTokenString(":");
                    return tk;
                } else if (c == ';') {
                    tk.name = createTokenString("TK_SEM");
                    tk.lexeme_value = createTokenString(";");
                    return tk;
                } else if (c == ' ' || c == '\t') {
                    // Skip whitespace
                    continue;
                } else {
                    // Handle identifiers and keywords
                    if (isalpha(c) || c == '_' || c == '#') {
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
                            tk.lexeme_value = createTokenString(lexeme);
                        } else {
                            // It's an identifier
                            if (lexeme[0] == '_') {
                                tk.name = createTokenString("TK_FUNID");
                            } else if (lexeme[0] == '#') {
                                tk.name = createTokenString("TK_RUID");
                            } else {
                                tk.name = createTokenString("TK_ID");
                            }
                            tk.lexeme_value = createTokenString(lexeme);
                        }
                        return tk;
                    }
                    
                    // Handle numbers
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
                        return tk;
                    }
                    
                    // Handle unknown characters
                    char error_msg[100];
                    snprintf(error_msg, sizeof(error_msg), "Unknown character: %c", c);
                    tk.name = createTokenString("ERROR");
                    tk.lexeme_value = createTokenString(error_msg);
                    return tk;
                }
            
            case 33: // Comment state
                c = getChar(fp);
                if (c == '\n') {
                    lineNo++;
                    tk.name = createTokenString("TK_COMMENT");
                    tk.lexeme_value = createTokenString("%");
                    return tk;
                } else if (c == EOF) {
                    tk.name = createTokenString("TK_COMMENT");
                    tk.lexeme_value = createTokenString("%");
                    return tk;
                }
                state = 33;
                break;
            
            // Add other states here
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

// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         printf("Usage: %s <testfile>\n", argv[0]);
//         return 1;
//     }

//     FILE *fp = fopen(argv[1], "r");
//     if (!fp) {
//         printf("Error: Could not open file %s\n", argv[1]);
//         return 1;
//     }
// 	initLexer();
//     // Print all tokens in the file
//     printTokens(fp);

//     fclose(fp);
//     return 0;
// }