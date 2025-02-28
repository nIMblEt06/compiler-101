#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char buffer[BUFFER_SIZE];
extern Table *symbol_table;
extern int lexemeBegin;
extern int retraction;
extern int buffer_ptr;
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
    buffer_ptr -= num;
    if (buffer_ptr < 0) {
        buffer_ptr += BUFFER_SIZE;
    }
    retraction += num;
}

void initLexer() {
    buffer_ptr = 0;
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

token getNextToken(FILE* fp) {
    token tk;
    state = 0;
    tk.line = lineNo;
	while(true){
		switch (state)
		{
		case 0:
			char c = getChar(fp);
			if(c=='+'){
				state = 1;
			}
			else if(c=='-'){
				state = 2;
			}
			else if(c=='/'){
				state = 4;
			}
			else if(c=='('){
				state = 5;
			}
			else if(c==')'){
				state = 6;
			}
			else if(c=='['){
				state = 7;
			}
			else if(c==']'){
				state = 8;
			}
			else if(c==','){
				state = 9;
			}
			else if(c=='.'){
				state = 10;
			}
			else if(c==':'){
				state = 11;
			}
			else if(c==';'){
				state = 12;
			}
			else if(c=='~'){
				state = 13;
			}
			else if(c=='!'){
				state = 14;
			}
			else if(c=='='){
				state = 16;
			}
			else if(c=='@'){
				state = 18;
			}
			else if(c=='&'){
				state = 21;
			}
			else if(c=='<'){
				state = 24;
			}
			else if(c=='>'){
				state = 30;
			}
			else if(c=='%'){
				state = 33;
			}
			else if(c==' '||c=='\t'){
				state = 0;
			}
			else if(isalpha(c)){
				if(c=='b'||c=='c'||c=='d'){
					state = 35;
				}
				else{
					state = 39;
				}
			}
			else if(c=='_'){
				state=41;
			}
			else if(c=='#'){
				state=46;
			}
			else if(isdigit(c)){
				state  = 49;
			}
			else if(c=='\n'){
				state=59;
			}
			else{
				tk.name = "ERROR";
        		char *dest = get_name();
        		const char *sym = " is an Unknown Symbol";
        		strcat(dest, sym);
        		tk.lexeme_value = dest;
        		lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
        		return tk;
			}
			break;
		case 1:
			tk.name="TK_PLUS";
			tk.lexeme_value = "+";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 2:
			tk.name="TK_MINUS";
			tk.lexeme_value = "-";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 3:
			tk.name="TK_MUL";
			tk.lexeme_value = "*";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 4:
			tk.name="TK_DIV";
			tk.lexeme_value = "/";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 5:
			tk.name="TK_OP";
			tk.lexeme_value = "(";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 6:
			tk.name="TK_CL";
			tk.lexeme_value = ")";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 7:
			tk.name="TK_SQL";
			tk.lexeme_value = "[";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 8:
			tk.name="TK_SQR";
			tk.lexeme_value = "]";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 9:
			tk.name="TK_COMMA";
			tk.lexeme_value = ",";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 10:
			tk.name="TK_";
			tk.lexeme_value = ".";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 11:
			tk.name="TK_COLON";
			tk.lexeme_value = ":";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 12:
			tk.name="TK_SEM";
			tk.lexeme_value = ";";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 13:
			tk.name="TK_NOT";
			tk.lexeme_value = "~";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 14:
			c = getChar(fp);
			if(c=='='){
				state = 15;
			}
			else{
				retract(1);
				tk.name = "ERROR";
        		char *dest = get_name();
        		const char *sym = " is an Unknown Symbol";
        		strcat(dest, sym);
        		tk.lexeme_value = dest;
        		lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
        		return tk;
			}
			break;
		case 15:
			break;
		case 16:
			break;
		case 17:
			break;
		case 18:
			break;
		case 19:
			break;
		case 20:
			break;
		case 21:
			break;
		case 22:
			break;
		case 23:
			break;
		case 24:
			break;
		case 25:
			break;
		case 26:
			break;
		case 27:
			break;
		case 28:
			break;
		case 29:
			break;
		case 30:
			break;
		case 31:
			break;
		case 32:
			break;
		case 33:
			break;
		case 34:
			break;
		case 35:
			break;
		case 36:
			break;
		case 37:
			break;
		case 38:
			break;
		case 39:
			break;
		case 40:
			break;
		case 41:
			break;
		case 42:
			break;
		case 43:
			break;
		case 44:
			break;
		case 45:
			break;
		case 46:
			break;
		case 47:
			break;
		case 48:
			break;
		case 49:
			break;
		case 50:
			break;
		case 51:
			break;
		case 52:
			break;
		case 53:
			break;
		case 54:
			break;
		case 55:
			break;
		case 56:
			break;
		case 57:
			break;
		case 58:
			break;
		case 59:
			break;
		default:
			break;
		}
	}



}

void printToken(char *testfile) {
    
}