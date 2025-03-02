#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

// Helper function to create a terminal node
static parse_tree create_terminal_node(token tk) {
    sym symbol = {.isTerminal = true};
    // Map token name to Terminal enum
    for (int i = 0; i < MAX_TERMINALS; i++) {
        if (Terminals[i] && strcmp(tk.name, Terminals[i]) == 0) {
            symbol.t = (Terminal)i;
            break;
        }
    }
    parse_tree node = create_node(symbol);
    node->tok = tk;
    return node;
}

// Helper function to create a non-terminal node
static parse_tree create_non_terminal_node(Non_terminal nt) {
    sym symbol = {.isTerminal = false, .nT = nt};
    return create_node(symbol);
}

void handle_parse_error(token current_token, Non_terminal current_nt) {
    printf("Parse Error at line %d: Unexpected token %s ", 
           current_token.line, current_token.name);
    printf("while processing non-terminal %s\n", nonTerminals[current_nt]);
}

parse_tree create_parse_tree(FILE* fp) {
    // Initialize lexer and parse table
    initLexer();
    
    // Initialize first_and_follow_table
    first_and_follow_table = (FIRST_AND_FOLLOW_ENTRY*)malloc(HASH_TABLE_SIZE * sizeof(FIRST_AND_FOLLOW_ENTRY));
    if (!first_and_follow_table) {
        printf("Error: Failed to allocate memory for first_and_follow_table\n");
        return NULL;
    }
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        first_and_follow_table[i].token_name = NULL;
        first_and_follow_table[i].first_set = NULL;
        first_and_follow_table[i].follow_set = NULL;
    }
    
    // Initialize grammar
    fill_grammar();
    compute_parse_table();
    
    // Create root node (program is the start symbol)
    parse_tree root = create_non_terminal_node(program);
    if (!root) {
        printf("Error: Failed to create root node\n");
        return NULL;
    }
    
    // Stack for parsing - using dynamic array for simplicity
    parse_tree* stack = malloc(1000 * sizeof(parse_tree));
    if (!stack) {
        printf("Error: Failed to allocate stack\n");
        free_parse_tree(root);
        return NULL;
    }
    int stack_top = 0;
    
    // Push root to stack
    stack[stack_top++] = root;
    
    // Get first token
    token current_token = getNextToken(fp);
    
    // Main parsing loop
    while (stack_top > 0 && current_token.name != NULL) {
        // Get top of stack
        parse_tree current_node = stack[--stack_top];
        if (!current_node) continue;
        
        if (current_node->symbol.isTerminal) {
            // Terminal node - match with input
            Terminal current_terminal = get_terminal(current_token.name);
            if (current_terminal == -1) {
                printf("Error: Unknown terminal token %s\n", current_token.name);
                current_token = getNextToken(fp);
                continue;
            }
            
            if (current_node->symbol.t == current_terminal) {
                current_node->tok = current_token;
                current_token = getNextToken(fp);
            } else {
                // Improved error recovery
                if (current_terminal == TK_END || current_terminal == TK_MAIN) {
                    // Special handling for synchronization tokens
                    printf("Error: Expected %s but got %s - synchronizing\n", 
                           Terminals[current_node->symbol.t],
                           current_token.name);
                    stack_top = 0;  // Clear stack for resynchronization
                } else {
                    printf("Error: Expected %s but got %s\n", 
                           Terminals[current_node->symbol.t],
                           current_token.name);
                    current_token = getNextToken(fp);
                }
            }
        } else {
            // Non-terminal node - expand using parse table
            Terminal current_t = get_terminal(current_token.name);
            if (current_t == -1) {
                printf("Error: Unknown terminal token %s\n", current_token.name);
                current_token = getNextToken(fp);
                continue;
            }
            
            parse_table_entry entry = get_parse_table_entry(current_node->symbol.nT, current_t);
            
            if (entry.is_error) {
                // Improved error handling for specific cases
                if (current_node->symbol.nT == returnStmt && 
                    (current_t == TK_END || current_t == TK_MAIN)) {
                    // Handle empty return statement
                    continue;
                } else if (current_node->symbol.nT == otherStmts && current_t == TK_END) {
                    // Handle empty otherStmts
                    continue;
                } else {
                    printf("Error: No production rule for %s with lookahead %s\n",
                           nonTerminals[current_node->symbol.nT],
                           current_token.name);
                    current_token = getNextToken(fp);
                    continue;
                }
            }
            
            // Get the rule to expand with
            RULE rule = Grammar[entry.rule_number];
            current_node->rule_number = entry.rule_number;
            
            // Create child nodes in reverse order (for stack)
            for (int i = rule.rhs_count - 1; i >= 0; i--) {
                parse_tree child;
                if (rule.rhs[i].isTerminal) {
                    child = create_node(rule.rhs[i]);
                } else {
                    child = create_non_terminal_node(rule.rhs[i].nT);
                }
                
                if (!child) continue;
                
                add_child(current_node, child);
                
                // Push to stack if not epsilon
                if (!(rule.rhs[i].isTerminal && rule.rhs[i].t == EPSILON)) {
                    if (stack_top < 1000) {
                        stack[stack_top++] = child;
                    }
                }
            }
        }
    }
    
    free(stack);
    return root;
}
