#include "symbolTable.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *nonTerminals[] = {"program",
                        "mainFunction",
                        "otherFunctions",
                        "function",
                        "input_par",
                        "output_par",
                        "parameter_list",
                        "dataType",
                        "primitiveDatatype",
                        "constructedDatatype",
                        "remaining_list",
                        "stmts",
                        "typeDefinitions",
                        "actualOrRedefined",
                        "typeDefinition",
                        "fieldDefinitions",
                        "fieldDefinition",
                        "fieldtype",
                        "moreFields",
                        "declarations",
                        "declaration",
                        "global_or_not",
                        "otherStmts",
                        "stmt",
                        "assignmentStmt",
                        "singleOrRecId",
                        "constructedVariable",
                        "oneExpansion",
                        "moreExpansions",
                        "funCallStmt",
                        "outputParameters",
                        "inputParameters",
                        "iterativeStmt",
                        "conditionalStmt",
                        "elsePart",
                        "ioStmt",
                        "arithmeticExpression",
                        "expPrime",
                        "term",
                        "termPrime",
                        "factor",
                        "highPrecedenceOperator",
                        "lowPrecedenceOperators",
                        "booleanExpression",
                        "var",
                        "logicalOp",
                        "relationalOp",
                        "returnStmt",
                        "optionalReturn",
                        "idList",
                        "more_ids",
                        "definetypestmt",
                        "A",
                        "option_single_constructed",
                        NULL};

const char *Terminals[] = {
    "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM",
    "TK_RNUM", "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS",
    "TK_END", "TK_WHILE", "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE",
    "TK_AS", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER",
    "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT",
    "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON",
    "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF",
    "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN",
    "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL",
    "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR",
    "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT",
    "TK_GE", "TK_NE", "TK_DOLLAR", "ERROR", "TK_DELIM",
    "EPSILON"};

long long hashFunc(char *inp) {
    // we use the rolling polynomial hash method with formula s[i]*p^i mod m
    int prime =
        53;  // prime number close to number of alphabets(upper and lowercase)
    long long power = 1;
    int modulo =
        1e5 + 3;  // large prime number (probability of collision is 1/modulo)
    long long res = 0;  // final key value to be returned
    for (int i = 0; i < strlen(inp); i++) {
        res = (((inp[i] - '0' + 1) * power) + res) % modulo;
        power = (power * prime) % modulo;
    }
    return res;
}
// this function is to create the table and initialize all the entries to null
Table *createTable() {
    int sz = tableSize * sizeof(Entry);
    Table *symTable = malloc(sz);
    for (int i = 0; i < tableSize; i++) {
        symTable->entries[i] = NULL;
    }

    return symTable;
}

Entry *createEntry(char *value, char *token) {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    entry->value = (char *)malloc(strlen(value) + 1);
    entry->token = (char *)malloc(strlen(token) + 1);
    strcpy(entry->value, value);
    strcpy(entry->token, token);
    return entry;
}
char *search(Table *table, char *value) {
    long long hashval = hashFunc(value);
    if (table->entries[hashval] != NULL &&
        strcmp(table->entries[hashval]->value, value) == 0)
        return table->entries[hashval]->token;
    else
        return "\0";
}
void insert(Table *table, char *value, char *token) {
    Entry *item = createEntry(value, token);

    int hashval = hashFunc(value);
    while (table->entries[hashval] != NULL) {
        hashval = (hashval + 1) % tableSize;
    }
    table->entries[hashval] = item;
}