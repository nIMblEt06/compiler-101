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
    lineNo = 0;
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
	char c;
	int lex_len=0;
    tk.line = lineNo;
	while(true){
		switch (state)
		{
		case 0:
			c = getChar(fp);
			if(c=='+'){
				state = 1;
			}
			else if(c=='-'){
				state = 2;
			}
			else if(c=='*'){
				state = 3;
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
				state = 60;
			}
			else if(islower(c)){
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
			else if(c==EOF){
				tk.name = "EOF";
				tk.lexeme_value = "EOF";
				return tk;
			}
			else{
				tk.name = "ERROR";
        		char *dest = getName();
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
        		char *dest = getName();
        		const char *sym = " is an Unknown Symbol";
        		strcat(dest, sym);
        		tk.lexeme_value = dest;
        		lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
        		return tk;
			}
			break;
		case 15:
			tk.name="TK_NE";
			tk.lexeme_value = "!=";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 16:
			c = getChar(fp);
			if(c=='='){
				state = 17;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 17:
			tk.name="TK_EQ";
			tk.lexeme_value = "==";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 18:
			c = getChar(fp);
			if(c=='@'){
				state = 19;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 19:
			c = getChar(fp);
			if(c=='@'){
				state = 20;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 20:
			tk.name="TK_OR";
			tk.lexeme_value = "@@@";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 21:
			c = getChar(fp);
			if(c=='&'){
				state = 22;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 22:
			c = getChar(fp);
			if(c=='&'){
				state = 23;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 23:
			tk.name="TK_AND";
			tk.lexeme_value = "&&&";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 24:
			c = getChar(fp);
			if(c=='-'){
				state = 25;
			}
			else if(c=='='){
				state = 28;
			}
			else{
				state = 29;
			}
			break;
		case 25:
			c = getChar(fp);
			if(c=='-'){
				state = 26;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 26:
			c = getChar(fp);
			if(c=='-'){
				state = 27;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Symbol";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 27:
			tk.name="TK_ASSIGNOP";
			tk.lexeme_value = "<---";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 28:
			tk.name="TK_LE";
			tk.lexeme_value = "<=";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 29:
			retract(1);
			tk.name="TK_LT";
			tk.lexeme_value = "<";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 30:
			c = getChar(fp);
			if(c=='='){
				state = 31;
			}
			else{
				state = 32;
			}
			break;
			break;
		case 31:
			tk.name="TK_GE";
			tk.lexeme_value = ">=";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 32:
			retract(1);
			tk.name="TK_GT";
			tk.lexeme_value = ">";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			return tk;
			break;
		case 33:
			c = getChar(fp);
			if(c=='\n'){
				state = 34;
			}
			else{
				state = 33;
			}
			break;
		case 34:
			tk.name="TK_COMMENT";
			tk.lexeme_value = "%";
			lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
			lineNo++;
			return tk;
			break;
		case 35:
			c = getChar(fp);
			if(islower(c)){
				state = 39;
			}
			else if(c>='2'&&c<='7'){
				state  = 36;
			}
			else{
				state = 40;
			}
			break;
		case 36:
			c = getChar(fp);
			if(c=='b'||c=='c'||c=='d'){
				state = 36;
			}
			else if(c>='2'&&c<='7'){
				state = 37;
			}
			else{
				state = 38;
			}
			break;
		case 37:
			c = getChar(fp);
			if(c>='2'&&c<='7'){
				state = 37;
			}
			else{
				state = 38;
			}
			break;
		case 38:
		//have to figure How to handle scope.
			retract(1);
			lex_len = getLen();
			if(lex_len>=2 && lex_len<=20){
				tk.name = "TK_ID";
				tk.lexeme_value = getName();
				lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
				return tk;
			}
			else{
				tk.name = "ERROR";
				if(lex_len<2){
					tk.lexeme_value = "identifier shorter than 2";
				}
				if(lex_len>20){
					tk.lexeme_value = "identifier longer than 20";
				}
				lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
				return tk;
			}
			break;
		case 39:
			c = getChar(fp);
			if(islower(c)){
				state = 39;
			}
			else{
				state =40;
			}
			break;
		case 40:
			retract(1);
			tk.lexeme_value = getName();
			char *keyword = search(symbol_table,tk.lexeme_value);
			if(strcmp(keyword,"\0")==0){
				tk.name = "TK_FIELDID";
			}
			else{
				tk.name = keyword;
			}
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
      		return tk;
			break;
		case 41:
			c = getChar(fp);
			if(isalpha(c)){
				state = 42;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 42:
			c = getChar(fp);
			if(isalpha(c)){
				state=42;
			}
			else if(isdigit(c)){
				state = 43;
			}
			else{
				state = 45;
			}
			break;
		case 43:
			c = getChar(fp);
			if(isdigit(c)){
				state = 43;
			}
			else{
				state =44;
			}
			break;
		case 44:
			retract(1);
			tk.name = "TK_FUNID";
			tk.lexeme_value = getName();
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 45:
			retract(1);
			lex_len = getLen();
			if(lex_len<=30){
				tk.lexeme_value = getName();
				char *keyword = search(symbol_table,tk.lexeme_value);
				if(strcmp(keyword,"\0")==0){
					tk.name = "TK_FUNID";
				}
				else{
					tk.name = keyword;
				}
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
      			return tk;
			}
			else {
				tk.name = "ERROR";
				tk.lexeme_value = "FUNID cannot be longer than 30";
				lexemeBegin = (buffer_ptr)%(BUFFER_SIZE);
				return tk;
			}
			break;
		case 46:
			c = getChar(fp);
			if(islower(c)){
				state = 47;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 47:
			c = getChar(fp);
			if(islower(c)){
				state=47;
			}
			else{
				state=48;
			}
			break;
		case 48:
			retract(1);
			tk.name = "TK_RUID";
			tk.lexeme_value = getName();
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 49:
			c = getChar(fp);
			if(isdigit(c)){
				state=49;
			}
			else if(c=='.'){
				state = 51;
			}
			else{
				state=50;
			}
			break;
		case 50:
			retract(1);
			tk.name = "TK_NUM";
			tk.num = atoi(getName());
			// printf(" %d \n",tk.num);
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 51:
			//if we do not encounter a number do we retract and return "ERROR" or TK_NUM
			//Currently only returning "ERROR" but maybe need of consideration
			c = getChar(fp);
			if(isdigit(c)){
				state=52;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 52:
			c = getChar(fp);
			if(isdigit(c)){
				state=53;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 53:
			c = getChar(fp);
			if(c=='E'){
				state=55;
			}
			else{
				state=54;
			}
			break;
		case 54:
			retract(1);
			tk.name = "TK_RNUM";
			tk.rnum = atof(getName());
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 55:
			c = getChar(fp);
			if(isdigit(c)){
				state = 57;
			}
			else if(c=='+'||c=='-'){
				state = 56;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 56:
			c = getChar(fp);
			if(isdigit(c)){
				state = 57;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 57:
			c = getChar(fp);
			if(isdigit(c)){
				state = 58;
			}
			else{
				retract(1);
				tk.name = "ERROR";
				char *dest = getName();
				const char *sym = " is an Unknown Pattern";
				strcat(dest, sym);
				tk.lexeme_value = dest;
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
			break;
		case 58:
			tk.name = "TK_RNUM";
			tk.rnum = atof(getName());
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 59:
			tk.name = "LINE_BREAK";
			tk.lexeme_value = "\\n";
			lineNo++;
			lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
			return tk;
			break;
		case 60:
			c= getChar(fp);
			if(c==' '||c=='\t'){
				state = 60;
			}
			else{
				retract(1);
				tk.name = "SPACE";
				tk.lexeme_value = getName();
				lexemeBegin = (buffer_ptr) % (BUFFER_SIZE);
				return tk;
			}
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