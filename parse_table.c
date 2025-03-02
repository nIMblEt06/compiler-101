/******************************************************************************
 * This file along with parse_table.h handles all the operations related to parse_table
 * Import this as module in parser.c/parser.h
*******************************************************************************/

#include "parse_table.h"
#include <stdio.h>
#include <stdlib.h>

// Define the parse table
parse_table_entry parse_table[MAX_NON_TERMINALS][MAX_TERMINALS];

// Helper function to check if a terminal is in a set
bool is_terminal_in_set(set* s, Terminal t) {
    if (!s) return false;
    for (int i = 0; i < s->size; i++) {
        if (s->t[i] == t) return true;
    }
    return false;
}

void initialize_parse_table() {
    // Initialize all entries as errors
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        for (int j = 0; j < MAX_TERMINALS; j++) {
            parse_table[i][j].is_error = true;
            parse_table[i][j].rule_number = -1;
        }
    }
}

void compute_parse_table() {
    initialize_parse_table();
    
    // For each grammar rule
    for (int rule_num = 0; rule_num < rule_cnt; rule_num++) {
        RULE current_rule = Grammar[rule_num];
        Non_terminal lhs_nt = current_rule.lhs.nT;
        
        // Create a symbol for the LHS non-terminal
        sym lhs_sym = {.isTerminal = false, .nT = lhs_nt};
        
        // Get FIRST set of the RHS
        bool has_epsilon = true;
        set* first_of_rhs = NULL;
        
        // If RHS starts with a terminal
        if (current_rule.rhs[0].isTerminal) {
            // Create a new set with just this terminal
            first_of_rhs = create_set();
            add_to_set(first_of_rhs, current_rule.rhs[0].t);
            has_epsilon = (current_rule.rhs[0].t == EPSILON);
        } else {
            // For non-terminals, compute FIRST set
            first_of_rhs = first_set(current_rule.rhs[0]);
            has_epsilon = first_of_rhs ? first_of_rhs->containsEpsilon : false;
        }
        
        // For each terminal in FIRST(RHS), add rule to parse table
        if (first_of_rhs) {
            for (int i = 0; i < first_of_rhs->size; i++) {
                Terminal t = first_of_rhs->t[i];
                if (t != EPSILON) {
                    parse_table[lhs_nt][t].is_error = false;
                    parse_table[lhs_nt][t].rule_number = rule_num;
                }
            }
        }
        
        // If RHS can derive epsilon, add rule to FOLLOW(LHS) entries
        if (has_epsilon) {
            set* follow_of_lhs = follow_set(lhs_sym);
            if (follow_of_lhs) {
                for (int i = 0; i < follow_of_lhs->size; i++) {
                    Terminal t = follow_of_lhs->t[i];
                    parse_table[lhs_nt][t].is_error = false;
                    parse_table[lhs_nt][t].rule_number = rule_num;
                }
            }
        }
    }
}

parse_table_entry get_parse_table_entry(Non_terminal nt, Terminal t) {
    return parse_table[nt][t];
}

void print_parse_table() {
    printf("\nLL(1) Parse Table:\n");
    printf("%-20s", "Non-terminal");
    
    // Print terminal headers
    for (int t = 0; t < MAX_TERMINALS; t++) {
        if (Terminals[t] == NULL) continue;
        printf("%-8s ", Terminals[t]);
    }
    printf("\n");
    
    // Print separator line
    for (int i = 0; i < 20 + (8 * MAX_TERMINALS); i++) printf("-");
    printf("\n");
    
    // Print table contents
    for (int nt = 0; nt < MAX_NON_TERMINALS; nt++) {
        if (nonTerminals[nt] == NULL) continue;
        
        printf("%-20s", nonTerminals[nt]);
        for (int t = 0; t < MAX_TERMINALS; t++) {
            if (Terminals[t] == NULL) continue;
            
            if (parse_table[nt][t].is_error) {
                printf("%-8s ", "-");
            } else {
                printf("R%-7d ", parse_table[nt][t].rule_number);
            }
        }
        printf("\n");
    }
}

void report_parse_table_conflicts() {
    printf("\nChecking for parse table conflicts...\n");
    bool has_conflicts = false;
    
    for (int nt = 0; nt < MAX_NON_TERMINALS; nt++) {
        if (nonTerminals[nt] == NULL) continue;
        
        for (int t = 0; t < MAX_TERMINALS; t++) {
            if (Terminals[t] == NULL) continue;
            
            // Count how many rules apply to this cell
            int rules_for_cell = 0;
            for (int rule = 0; rule < rule_cnt; rule++) {
                RULE current_rule = Grammar[rule];
                if (current_rule.lhs.nT == nt) {
                    sym first_sym = current_rule.rhs[0];
                    if (first_sym.isTerminal && first_sym.t == t) {
                        rules_for_cell++;
                    }
                }
            }
            
            if (rules_for_cell > 1) {
                has_conflicts = true;
                printf("Conflict at [%s, %s]: %d rules apply\n",
                       nonTerminals[nt], Terminals[t], rules_for_cell);
            }
        }
    }
    
    if (!has_conflicts) {
        printf("No conflicts found. Grammar is LL(1).\n");
    }
}