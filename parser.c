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
    compute_parse_table();
    
    // Create root node (program is the start symbol)
    parse_tree root = create_non_terminal_node(program);
    
    // Stack for parsing - using dynamic array for simplicity
    parse_tree* stack = malloc(1000 * sizeof(parse_tree));
    int stack_top = 0;
    
    // Push root to stack
    stack[stack_top++] = root;
    
    // Get first token
    token current_token = getNextToken(fp);
    
    // Main parsing loop
    while (stack_top > 0) {
        // Get top of stack
        parse_tree current_node = stack[--stack_top];
        
        if (current_node->symbol.isTerminal) {
            // Terminal node - match with input
            if (current_node->symbol.t == get_terminal(current_token.name)) {
                current_node->tok = current_token;
                current_token = getNextToken(fp);
            } else {
                handle_parse_error(current_token, current_node->parent->symbol.nT);
                free(stack);
                return NULL;
            }
        } else {
            // Non-terminal node - expand using parse table
            Non_terminal current_nt = current_node->symbol.nT;
            Terminal current_t = get_terminal(current_token.name);
            
            parse_table_entry entry = get_parse_table_entry(current_nt, current_t);
            
            if (entry.is_error) {
                handle_parse_error(current_token, current_nt);
                free(stack);
                return NULL;
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
                add_child(current_node, child);
                
                // Push to stack if not epsilon
                if (!(rule.rhs[i].isTerminal && rule.rhs[i].t == EPSILON)) {
                    stack[stack_top++] = child;
                }
            }
        }
    }
    
    free(stack);
    return root;
}
