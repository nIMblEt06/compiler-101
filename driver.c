/**********************************
            |GROUP-06|
***********************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "first_and_follow.h"
#include "grammar.h"
#include "parser.h"
#include "parse_table.h"

// External declarations
extern int rule_cnt;

// Function declarations
void display_comment_free_code(const char* clean_file);
void parse_and_print_tree(const char* clean_file);
void measure_execution_time(const char* clean_file);
void display_menu();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    // Create a clean version of the input file without comments
    char clean_file[256];
    snprintf(clean_file, sizeof(clean_file), "%s.clean", argv[1]);
    removeComments(argv[1], clean_file);

    int option = -1;
    
    // Display menu and process options in a loop
    while (option != 0) {
        display_menu();
        
        // Get user input
        printf("Enter your choice: ");
        scanf("%d", &option);
        
        switch (option) {
            case 0:
                printf("Exiting program.\n");
                break;
            case 1:
                display_comment_free_code(clean_file);
                break;
            case 2: {
                // Use the existing printTokens function
                FILE* fp = fopen(argv[1], "r");
                if (!fp) {
                    printf("Error: Could not open clean file\n");
                    break;
                }
                initLexer();
                printTokens(fp);
                fclose(fp);
                break;
            }
            case 3:
                parse_and_print_tree(clean_file);
                break;
            case 4:
                measure_execution_time(clean_file);
                break;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

    return 0;
}

void display_menu() {
    printf("\n");
    printf("0 : For exit\n");
    printf("1 : For removal of comments - print the comment free code on the console\n");
    printf("2 : For printing the token list (on the console) generated by the lexer. \n");
    printf("3 : For parsing to verify the syntactic correctness of the input source code and printing the parse tree appropriately. \n");
    printf("4: For printing (on the console) the total time taken by our compiler\n");
    printf("\n");
}

void display_comment_free_code(const char* clean_file) {
    // Open the clean file and print its contents
    FILE* fp = fopen(clean_file, "r");
    if (!fp) {
        printf("Error: Could not open clean file\n");
        return;
    }
    
    printf("\n--- Comment-free code ---\n");
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    
    fclose(fp);
}

void parse_and_print_tree(const char* clean_file) {
    FILE* fp = fopen(clean_file, "r");
    if (!fp) {
        printf("Error: Could not open clean file\n");
        return;
    }
    
    // Initialize grammar and parse table
    initializeHashTable();
    fill_grammar();
    compute_parse_table();
    
    // Create parse tree
    parse_tree tree = create_parse_tree(fp);
    if (tree) {
        printf("\nParse Tree Structure:\n");
        printf("====================\n\n");
        print_parse_tree(tree, 0);
        
        // Validate tree structure
        printf("\nTree Validation:\n");
        printf("===============\n");
        
        // Print validation details
        if (tree->symbol.isTerminal) {
            printf("Terminal: %s", Terminals[tree->symbol.t]);
            if (tree->tok.lexeme_value) {
                printf(" (Value: %s)", tree->tok.lexeme_value);
            }
            printf("\n");
        } else {
            printf("Non-terminal: %s\n", nonTerminals[tree->symbol.nT]);
            if (tree->rule_number >= 0) {
                printf("  Using Rule %d: ", tree->rule_number);
                RULE rule = Grammar[tree->rule_number];
                printf("%s -> ", nonTerminals[rule.lhs.nT]);
                for (int i = 0; i < rule.rhs_count; i++) {
                    if (rule.rhs[i].isTerminal) {
                        printf("%s ", Terminals[rule.rhs[i].t]);
                    } else {
                        printf("%s ", nonTerminals[rule.rhs[i].nT]);
                    }
                }
                printf("\n");
            }
        }
        
        // Recursively validate children
        for (int i = 0; i < tree->num_children; i++) {
            if (tree->children[i]) {
                if (tree->children[i]->symbol.isTerminal) {
                    printf("Terminal: %s", Terminals[tree->children[i]->symbol.t]);
                    if (tree->children[i]->tok.lexeme_value) {
                        printf(" (Value: %s)", tree->children[i]->tok.lexeme_value);
                    }
                    printf("\n");
                } else {
                    printf("Non-terminal: %s\n", nonTerminals[tree->children[i]->symbol.nT]);
                    if (tree->children[i]->rule_number >= 0) {
                        printf("  Using Rule %d: ", tree->children[i]->rule_number);
                        RULE rule = Grammar[tree->children[i]->rule_number];
                        printf("%s -> ", nonTerminals[rule.lhs.nT]);
                        for (int j = 0; j < rule.rhs_count; j++) {
                            if (rule.rhs[j].isTerminal) {
                                printf("%s ", Terminals[rule.rhs[j].t]);
                            } else {
                                printf("%s ", nonTerminals[rule.rhs[j].nT]);
                            }
                        }
                        printf("\n");
                    }
                }
            }
        }
        
        free_parse_tree(tree);
    } else {
        printf("Failed to create parse tree\n");
    }
    
    fclose(fp);
}

void measure_execution_time(const char* clean_file) {
    clock_t start_time, end_time;
    double total_cpu_time, total_cpu_time_in_seconds;
    
    printf("\n--------------------------------------------------------------------------------------------------------\n");
    
    // Start timing
    start_time = clock();
    
    // Initialize lexer, grammar, and parse table
    initLexer();
    initializeHashTable();
    fill_grammar();
    compute_parse_table();
    
    // Open file
    FILE* fp = fopen(clean_file, "r");
    if (!fp) {
        printf("Error: Could not open clean file\n");
        return;
    }
    
    // Create parse tree
    parse_tree tree = create_parse_tree(fp);
    if (tree) {
        free_parse_tree(tree);
    }
    
    fclose(fp);
    
    // End timing
    end_time = clock();
    
    // Calculate time
    total_cpu_time = (double)(end_time - start_time);
    total_cpu_time_in_seconds = total_cpu_time / CLOCKS_PER_SEC;
    
    // Print timing results
    printf("Total CPU time: %f\n", total_cpu_time);
    printf("Total CPU time in seconds: %f\n", total_cpu_time_in_seconds);
    
    printf("--------------------------------------------------------------------------------------------------------\n");
}