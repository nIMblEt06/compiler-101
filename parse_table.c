/******************************************************************************
 * This file along with parse_table.h handles all the operations related to parse_table
 * Import this as module in parser.c/parser.h
*******************************************************************************/

#include "parse_table.h"

void create_parse_table(){
    for(int i = 0; i < GRAMMAR_MAX_SIZE; i++){
        if(Grammar[i].rhs[0].isTerminal == true){

        } else {
            
        }
    }
}