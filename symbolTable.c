/* -----------------------------------------
             Group Number- 6
ID 2021B5A71704P Name Arushi Gulati
ID 2021B3A71260P Name Aryan Rajkumar Keshri
ID 2021B4A70887P Name Chinmay Pushkar
ID 2021B3A71102P Name Om Kotadiya Jain
ID 2021B3A71117P Name Riddhi Agarwal
ID 2021B5A70923P Name Shwetabh Niket
--------------------------------------------- */

#include "symbolTable.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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