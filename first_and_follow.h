/* -----------------------------------------
             Group Number- 6
ID 2021B5A71704P Name Arushi Gulati
ID 2021B3A71260P Name Aryan Rajkumar Keshri
ID 2021B4A70887P Name Chinmay Pushkar
ID 2021B3A71102P Name Om Kotadiya Jain
ID 2021B3A71117P Name Riddhi Agarwal
ID 2021B5A70923P Name Shwetabh Niket
--------------------------------------------- */

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

#define max_terminal 100
#define NT_SIZE 100
#define FIRST_AND_FOLLOW_LENGTH 100
// DECIDED ON THE BASIS OF THE NUMBER OF NON_TERMINALS - VERIFY ONCE

extern RULE Grammar[GRAMMAR_MAX_SIZE];
typedef struct set{
    bool containsEpsilon;
	Terminal t[max_terminal]; //first or follow set corresponding to each non_terminal
	int size;
} set;

// set first_set[NT_SIZE];
// set follow_set[NT_SIZE];

// Array for storing all the FIRST and FOLLOW
typedef struct first_n_follow_entry{
    char* token_name;
    set *first_set;
    set *follow_set;
} FIRST_AND_FOLLOW_ENTRY;

FIRST_AND_FOLLOW_ENTRY *first_and_follow_table;

FIRST_AND_FOLLOW_ENTRY *get_first_and_follow_entry(sym x);

set *follow_set_util(Non_terminal x);
// both function below compute the first and follow set and store them in above two array of same name;
set *first_set(sym x);

//follow set uses first_set
set *follow_set(sym x);

// Set operations
set* create_set();
void add_to_set(set* s, Terminal t);

#endif