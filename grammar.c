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
 * This file along with grammar.h handles all the operations related to grammar
 * Import this as module in parser.c/parser.h
 * 
*******************************************************************************/
#include "grammar.h"
#define MAX_BUFF 256

RULE Grammar[GRAMMAR_MAX_SIZE];

int rule_cnt=0;

Non_terminal get_non_terminal(char *str){
	// Count the number of non-terminals (54 as defined in MAX_NON_TERMINALS)
	for (int i = 0; i < MAX_NON_TERMINALS; i++) {
		if (nonTerminals[i] != NULL && strcmp(str, nonTerminals[i]) == 0){
			return (Non_terminal)i;
		}
	}
	return -1; // Not found
}

Terminal get_terminal(char *str){
	// Count the number of terminals (61 as defined in MAX_TERMINALS)
	for (int i = 0; i < MAX_TERMINALS; i++) {
		if (Terminals[i] != NULL && strcmp(str, Terminals[i]) == 0) {
			return (Terminal)i;
		}
	}
	return -1; // Not found
}

void print_grammar() {
    for (int i = 0; i < rule_cnt; i++) {
        printf("%d: %s -> ", i, nonTerminals[Grammar[i].lhs.nT]);
        int rhs_cnt = Grammar[i].rhs_count;
        
        for (int j = 0; j < rhs_cnt; j++) {
            if (Grammar[i].rhs[j].isTerminal) {
                printf("%s ", Terminals[Grammar[i].rhs[j].t]);
            } else {
                printf("%s ", nonTerminals[Grammar[i].rhs[j].t]);
            }
        }
        printf("\n");
    }
}
