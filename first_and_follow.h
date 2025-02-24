/******************************************************************************
 * This file along with first_and_follow.c handles all the operations related to first_and_follow
 * Import this as module in parser.c/parser.h
*******************************************************************************/
#ifndef FIRST_AND_FOLLOW_H
#define FIRST_AND_FOLLOW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "grammar.h"

#define max_terminal 50
#define NT_SIZE 50
#define FIRST_AND_FOLLOW_LENGTH 35
// DECIDED ON THE BASIS OF THE NUMBER OF NON_TERMINALS

extern RULE Grammar[GRAMMAR_MAX_SIZE];
typedef struct set{
    bool containsEpsilon;
	Terminal t[max_terminal]; //first or follow set corresponding to each non_terminal
	int size;
} set;

set first_set[NT_SIZE];
set follow_set[NT_SIZE];

// Array for storing all the FIRST and FOLLOW
typedef struct first_n_follow_entry{
    char* token_name;
    set first_set;
    set follow_set;
} FIRST_AND_FOLLOW_ENTRY;

FIRST_AND_FOLLOW_ENTRY *first_and_follow_table[FIRST_AND_FOLLOW_LENGTH];

// both function below compute the first and follow set and store them in above two array of same name;
set *first_set(sym x);

//follow set uses first_set
void follow_set(sym x);

#endif