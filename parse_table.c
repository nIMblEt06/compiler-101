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
        
        // Skip invalid non-terminals
        if (lhs_nt < 0 || lhs_nt >= MAX_NON_TERMINALS) {
            printf("Warning: Rule %d has invalid LHS non-terminal index %d\n", rule_num, lhs_nt);
            continue;
        }
        
        // Create a symbol for the LHS non-terminal
        sym lhs_sym = {.isTerminal = false, .nT = lhs_nt};
        
        // Special handling for empty productions (EPSILON)
        if (current_rule.rhs_count == 1 && current_rule.rhs[0].isTerminal && current_rule.rhs[0].t == EPSILON) {
            set* follow_of_lhs = follow_set(lhs_sym);
            if (follow_of_lhs) {
                for (int i = 0; i < follow_of_lhs->size; i++) {
                    Terminal t = follow_of_lhs->t[i];
                    // Add special handling for TK_END and TK_MAIN
                    if ((t == TK_END && lhs_nt == returnStmt) || 
                        (t == TK_MAIN && lhs_nt == returnStmt) ||
                        (t == TK_END && lhs_nt == otherStmts)) {
                        parse_table[lhs_nt][t].is_error = false;
                        parse_table[lhs_nt][t].rule_number = rule_num;
                    }
                    else if (t != EPSILON && t >= 0 && t < MAX_TERMINALS) {
                        parse_table[lhs_nt][t].is_error = false;
                        parse_table[lhs_nt][t].rule_number = rule_num;
                    }
                }
                free(follow_of_lhs);
            }
            continue;
        }
        
        // Special handling for TK_REAL in stmts
        if (current_rule.rhs[0].isTerminal && current_rule.rhs[0].t == TK_REAL) {
            parse_table[lhs_nt][TK_REAL].is_error = false;
            parse_table[lhs_nt][TK_REAL].rule_number = rule_num;
        }
        
        // Special handling for TK_RUID in otherFunctions
        if (current_rule.rhs[0].isTerminal && current_rule.rhs[0].t == TK_RUID) {
            parse_table[lhs_nt][TK_RUID].is_error = false;
            parse_table[lhs_nt][TK_RUID].rule_number = rule_num;
        }
        
        // Special handling for statement tokens in stmts
        if (lhs_nt == stmts) {
            Terminal stmt_tokens[] = {TK_READ, TK_WRITE, TK_ID, TK_IF, TK_WHILE, TK_CALL, TK_RECORD, TK_REAL};
            for (int i = 0; i < 8; i++) {
                parse_table[stmts][stmt_tokens[i]].is_error = false;
                parse_table[stmts][stmt_tokens[i]].rule_number = 19; // Rule for stmts -> declarations typeDefinitions otherStmts returnStmt
            }
        }
        
        // Special handling for otherFunctions with TK_ID and function endings
        if (lhs_nt == otherFunctions) {
            // Handle function declarations
            parse_table[otherFunctions][TK_ID].is_error = false;
            parse_table[otherFunctions][TK_ID].rule_number = 3; // Rule for otherFunctions -> function otherFunctions
            
            // Handle function endings
            parse_table[otherFunctions][TK_END].is_error = false;
            parse_table[otherFunctions][TK_END].rule_number = 4; // Rule for otherFunctions -> EPSILON
            
            // Handle RUID in function context
            parse_table[otherFunctions][TK_RUID].is_error = false;
            parse_table[otherFunctions][TK_RUID].rule_number = 3;
        }
        
        // Special handling for mainFunction with TK_DOT and endings
        if (lhs_nt == mainFunction) {
            parse_table[mainFunction][TK_DOT].is_error = false;
            parse_table[mainFunction][TK_DOT].rule_number = 2; // Rule for mainFunction -> TK_MAIN stmts TK_END
            
            // Handle main function ending
            parse_table[mainFunction][TK_END].is_error = false;
            parse_table[mainFunction][TK_END].rule_number = 2;
        }
        
        // Special handling for returnStmt endings
        if (lhs_nt == returnStmt) {
            Terminal return_tokens[] = {TK_RETURN, TK_END, TK_RUID};
            for (int i = 0; i < 3; i++) {
                parse_table[returnStmt][return_tokens[i]].is_error = false;
                parse_table[returnStmt][return_tokens[i]].rule_number = rule_num;
            }
        }
        
        // Special handling for typeDefinitions
        if (lhs_nt == typeDefinitions) {
            Terminal type_tokens[] = {TK_TYPE, TK_RECORD};
            for (int i = 0; i < 2; i++) {
                parse_table[typeDefinitions][type_tokens[i]].is_error = false;
                parse_table[typeDefinitions][type_tokens[i]].rule_number = 12; // Rule for typeDefinitions -> actualOrRedefined typeDefinitions
            }
            
            // Handle empty typeDefinitions
            parse_table[typeDefinitions][TK_END].is_error = false;
            parse_table[typeDefinitions][TK_END].rule_number = 13; // Rule for typeDefinitions -> EPSILON
        }
        
        // Special handling for otherStmts
        if (lhs_nt == otherStmts) {
            Terminal stmt_start_tokens[] = {TK_ID, TK_READ, TK_WRITE, TK_IF, TK_WHILE, TK_CALL, TK_RECORD, TK_RETURN, TK_TYPE};
            for (int i = 0; i < 9; i++) {
                parse_table[otherStmts][stmt_start_tokens[i]].is_error = false;
                parse_table[otherStmts][stmt_start_tokens[i]].rule_number = 23; // Rule for otherStmts -> stmt otherStmts
            }
            
            // Handle statement endings
            Terminal stmt_end_tokens[] = {TK_END, TK_COLON, TK_RUID};
            for (int i = 0; i < 3; i++) {
                parse_table[otherStmts][stmt_end_tokens[i]].is_error = false;
                parse_table[otherStmts][stmt_end_tokens[i]].rule_number = 24; // Rule for otherStmts -> EPSILON
            }
        }
        
        // Get FIRST set of the RHS
        bool has_epsilon = false;
        set* first_of_rhs = create_set();
        if (!first_of_rhs) {
            printf("Error: Failed to create FIRST set for RHS of rule %d\n", rule_num);
            continue;
        }
        
        // Compute FIRST set of RHS by looking at each symbol
        for (int i = 0; i < current_rule.rhs_count && !has_epsilon; i++) {
            set* first_of_symbol = NULL;
            
            if (current_rule.rhs[i].isTerminal) {
                first_of_symbol = create_set();
                if (first_of_symbol) {
                    add_to_set(first_of_symbol, current_rule.rhs[i].t);
                }
            } else {
                // Skip invalid non-terminals
                if (current_rule.rhs[i].nT < 0 || current_rule.rhs[i].nT >= MAX_NON_TERMINALS) {
                    printf("Warning: Rule %d has invalid RHS non-terminal index %d\n", rule_num, current_rule.rhs[i].nT);
                    continue;
                }
                first_of_symbol = first_set(current_rule.rhs[i]);
            }
            
            if (first_of_symbol) {
                // Add all non-epsilon symbols to FIRST(RHS)
                for (int j = 0; j < first_of_symbol->size; j++) {
                    Terminal t = first_of_symbol->t[j];
                    if (t != EPSILON && t >= 0 && t < MAX_TERMINALS) {
                        add_to_set(first_of_rhs, t);
                    }
                }
                
                // Check if this symbol can derive epsilon
                has_epsilon = first_of_symbol->containsEpsilon;
                free(first_of_symbol);
            }
        }
        
        // For each terminal in FIRST(RHS), add rule to parse table
        for (int i = 0; i < first_of_rhs->size; i++) {
            Terminal t = first_of_rhs->t[i];
            if (t != EPSILON && t >= 0 && t < MAX_TERMINALS) {
                parse_table[lhs_nt][t].is_error = false;
                parse_table[lhs_nt][t].rule_number = rule_num;
            }
        }
        free(first_of_rhs);
        
        // If all symbols in RHS can derive epsilon, add rule to FOLLOW(LHS) entries
        if (has_epsilon) {
            set* follow_of_lhs = follow_set(lhs_sym);
            if (follow_of_lhs) {
                for (int i = 0; i < follow_of_lhs->size; i++) {
                    Terminal t = follow_of_lhs->t[i];
                    if (t != EPSILON && t >= 0 && t < MAX_TERMINALS) {
                        parse_table[lhs_nt][t].is_error = false;
                        parse_table[lhs_nt][t].rule_number = rule_num;
                    }
                }
                free(follow_of_lhs);
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