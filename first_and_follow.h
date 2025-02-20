/******************************************************************************
 * This file along with first_and_follow.c handles all the operations related to first_and_follow
 * Import this as module in parser.c/parser.h
*******************************************************************************/
#include "grammar.h"
#define max_terminal 50
#define NT_SIZE 50

extern RULE Grammer[MAX_SIZE];
typedef struct set{
	Non_terminal nt;
	Terminal t[max_terminal]; //first or follow set corresponding to each non_terminal
	int size;
} set;

set first_set[NT_SIZE];
set follow_set[NT_SIZE];

// both function below compute the first and follow set and store them in above two array of same name;
void first_set(sym x);
//follow set uses first_set
void follow_set(sym x);
