/******************************************************************************
 * This file along with parse_table.c handles all the operations related to parse_table
 * 
*******************************************************************************/
#ifndef PARSE_TABLE_H
#define PARSE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first_and_follow.h"
#include "grammar.h"
#include "parserDef.h"
#include "lexer.h"

// Parse_Table Struct
RULE parse_table[NON_TERMINALS_COUNT][TERMINALS_COUNT];
void create_parse_table();
#endif
