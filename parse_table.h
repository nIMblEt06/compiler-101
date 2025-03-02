/******************************************************************************
 * This file along with parse_table.c handles all the operations related to LL(1) parse table
 * Import this as module in parser.c/parser.h
*******************************************************************************/
#ifndef PARSE_TABLE_H
#define PARSE_TABLE_H

#include "grammar.h"
#include "first_and_follow.h"
// Parse table entry can either be a rule number or an error
typedef struct {
    bool is_error;
    int rule_number;  // Index into Grammar array if not error
} parse_table_entry;

// The parse table is a 2D array indexed by [non-terminal][terminal]
extern parse_table_entry parse_table[MAX_NON_TERMINALS][MAX_TERMINALS];

// Function declarations
void initialize_parse_table();
void compute_parse_table();
parse_table_entry get_parse_table_entry(Non_terminal nt, Terminal t);
void print_parse_table();

// Helper function to check if a terminal is in a set
bool is_terminal_in_set(set* s, Terminal t);

// Error reporting
void report_parse_table_conflicts();

#endif
