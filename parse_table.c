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

    for (int rule_num = 0; rule_num < rule_cnt; rule_num++) {
        RULE current_rule = Grammar[rule_num];
        Non_terminal lhs_nt = current_rule.lhs.nT;

        set* rhs_first_set = create_set();

        // Compute the FIRST set for the RHS of the current rule
        for (int i = 0; i < current_rule.rhs_count; i++) {
            if (current_rule.rhs[i].isTerminal && current_rule.rhs[i].t != EPSILON) {
                add_to_set(rhs_first_set, current_rule.rhs[i].t);
                break;
            } else {
                set* first = first_set(current_rule.rhs[i]);

                if ((i == current_rule.rhs_count - 1) && first->containsEpsilon) {
                    for (int j = 0; j < first->size; j++) {
                        add_to_set(rhs_first_set, first->t[j]);
                    }
                    rhs_first_set->containsEpsilon = true;
                    break;
                }

                if (first->containsEpsilon) {
                    for (int j = 0; j < first->size; j++) {
                        if (first->t[j] != EPSILON) {
                            add_to_set(rhs_first_set, first->t[j]);
                        }
                    }
                    continue;
                } else {
                    for (int j = 0; j < first->size; j++) {
                        add_to_set(rhs_first_set, first->t[j]);
                    }
                    break;
                }
            }
        }

        // If rhs_first_set does not contain EPSILON
        if (!rhs_first_set->containsEpsilon) {
            for (int i = 0; i < rhs_first_set->size; i++) {
                parse_table[lhs_nt][rhs_first_set->t[i]].rule_number = rule_num;
                parse_table[lhs_nt][rhs_first_set->t[i]].is_error = false;
            }
        }
        // If rhs_first_set contains EPSILON, use follow_set of LHS
        else {
            sym lhs = {.isTerminal = false, .nT = lhs_nt};
            set* lhs_follow = follow_set(lhs);

            // Add rules for terminals in rhs_first_set (excluding EPSILON)
            for (int i = 0; i < rhs_first_set->size; i++) {
                if (rhs_first_set->t[i] != EPSILON) {
                    parse_table[lhs_nt][rhs_first_set->t[i]].rule_number = rule_num;
                    parse_table[lhs_nt][rhs_first_set->t[i]].is_error = false;
                }
            }

            // Add rules for terminals in follow_set of LHS
            for (int j = 0; j < lhs_follow->size; j++) {
                parse_table[lhs_nt][lhs_follow->t[j]].rule_number = rule_num;
                parse_table[lhs_nt][lhs_follow->t[j]].is_error = false;
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

void write_parse_table_to_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Write header row
    fprintf(fp, "Non-terminal");
    for (int t = 0; t < MAX_TERMINALS; t++) {
        if (Terminals[t] != NULL) {
            fprintf(fp, ",%s", Terminals[t]);
        }
    }
    fprintf(fp, "\n");

    // Write table content
    for (int nt = 0; nt < MAX_NON_TERMINALS; nt++) {
        if (nonTerminals[nt] == NULL) continue;

        fprintf(fp, "%s", nonTerminals[nt]);
        for (int t = 0; t < MAX_TERMINALS; t++) {
            if (Terminals[t] == NULL) continue;

            if (parse_table[nt][t].is_error) {
                fprintf(fp, ",-");  // Mark errors as "-"
            } else {
                fprintf(fp, ",R%d", parse_table[nt][t].rule_number);
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Parse table successfully written to %s\n", filename);
}
#define CSV_FILE "parse_table.csv"

// int main(){
// 	fill_grammar();
// 	compute_parse_table();
// 	write_parse_table_to_csv(CSV_FILE);
// }