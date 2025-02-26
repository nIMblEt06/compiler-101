#ifndef symbolTable
#define symbolTable
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define tableSize 100001
typedef struct Entry{
    char* value;
    char* token;
}Entry;
typedef struct Table{
    Entry* entries[tableSize];
}Table;
char* search(Table* table,char* value);

extern const char *Terminals[];
extern const char *nonTerminals[];


long long hashFunc(char *inp);
Table *createTable();
Entry *createEntry(char * value, char* token);
char* search(Table* table, char* value);
void insert(Table* table, char* value, char* token);


#endif