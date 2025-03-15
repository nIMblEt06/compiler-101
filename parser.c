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
    printf("Parse Error at line %d: Unexpected token %s ", current_token.line,
           current_token.name);
    printf("while processing non-terminal %s\n", nonTerminals[current_nt]);
}

parse_tree create_parse_tree(FILE* fp) {
    // Initialize lexer and parse table
    initLexer();

    // Initialize first_and_follow_table
    first_and_follow_table = (FIRST_AND_FOLLOW_ENTRY*)malloc(
        HASH_TABLE_SIZE * sizeof(FIRST_AND_FOLLOW_ENTRY));
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
        if (!current_node) {
            printf("Error: Current node is NULL------------\n");
            continue;
        }

        // Check if we've reached the end of file
        if (current_token.name != NULL &&
            strcmp(current_token.name, "EOF") == 0) {
            // End of file reached, break the loop
            break;
        }

        // Check if the current token is an error token
        if (current_token.name != NULL &&
            strcmp(current_token.name, "ERROR") == 0) {
            if (current_token.lexeme_value &&
                strstr(current_token.lexeme_value, "Unknown Pattern")) {
                char* pattern = current_token.lexeme_value;
                // Extract the pattern from the message
                char* extracted_pattern = NULL;
                if (sscanf(pattern, "%*[^<]<%[^>]>", extracted_pattern) == 1) {
                    printf("Line %d Error: Unknown pattern <%s>\n",
                           current_token.line, extracted_pattern);
                } else {
                    char* end = strstr(pattern, " is an Unknown Pattern");
                    if (end) {
                        *end = '\0';
                        printf("Line %d Error: Unknown pattern <%s>\n",
                               current_token.line, pattern);
                    } else {
                        printf("Line %d Error: Unknown pattern\n",
                               current_token.line);
                    }
                }
            } else if (current_token.lexeme_value &&
                       strstr(current_token.lexeme_value, "Invalid Symbol")) {
                printf("Line %d Error: %s\n", current_token.line,
                       current_token.lexeme_value ? current_token.lexeme_value
                                                  : "Unknown error");
            } else {
                printf("Line %d Error: %s\n", current_token.line,
                       current_token.lexeme_value ? current_token.lexeme_value
                                                  : "Unknown error");
            }
            // Skip this token and continue parsing
            current_token = getNextToken(fp);
            // Push the current node back onto the stack to retry with the next
            // token
            stack[stack_top++] = current_node;
            continue;
        }

        if (current_node->symbol.isTerminal) {
            // Terminal node - match with input
            Terminal current_terminal = get_terminal(current_token.name);
            if (current_token.name == "LINE_BREAK") {
                printf("EXCEPTION: Current token is a line break\n");
                current_token = getNextToken(fp);
            } else if (current_terminal == -1) {
                printf("Line %d Error: Unknown Symbol <%s>\n",
                       current_token.line,
                       current_token.lexeme_value ? current_token.lexeme_value
                                                  : "");
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
                    printf(
                        "Line %d Error: The token %s for lexeme %s does not "
                        "match with the expected token %s\n",
                        current_token.line, current_token.name,
                        current_token.lexeme_value ? current_token.lexeme_value
                                                   : "",
                        Terminals[current_node->symbol.t]);
                    stack_top = 0;  // Clear stack for resynchronization
                } else {
                    printf(
                        "Line %d Error: The token %s for lexeme %s does not "
                        "match with the expected token %s\n",
                        current_token.line, current_token.name,
                        current_token.lexeme_value ? current_token.lexeme_value
                                                   : "",
                        Terminals[current_node->symbol.t]);
                    current_token = getNextToken(fp);
                    // Don't push the current node back - we're skipping this
                    // terminal
                }
            }
        } else {
            // Non-terminal node - expand using parse table
            Terminal current_t = get_terminal(current_token.name);
            if (current_token.name == "LINE_BREAK") {
                printf("EXCEPTION: Current token is a line break\n");
                current_token = getNextToken(fp);
                // Push the current node back onto the stack to retry with the
                // next token
                stack[stack_top++] = current_node;
                continue;
            }  // should be else if
            else if (current_t == -1) {
                printf("Line %d Error: Unknown Symbol <%s>\n",
                       current_token.line,
                       current_token.lexeme_value ? current_token.lexeme_value
                                                  : "");
                current_token = getNextToken(fp);
                // Push the current node back onto the stack to retry with the
                // next token
                stack[stack_top++] = current_node;
                continue;
            }

            parse_table_entry entry =
                get_parse_table_entry(current_node->symbol.nT, current_t);

            if (entry.is_error) {
                // Improved error handling for specific cases
                if (current_node->symbol.nT == returnStmt &&
                    (current_t == TK_END || current_t == TK_MAIN)) {
                    // Handle empty return statement
                    continue;
                } else if (current_node->symbol.nT == otherStmts &&
                           current_t == TK_END) {
                    // Handle empty otherStmts
                    continue;
                } else {
                    // printf(
                    //     "Line %d Error: Invalid token %s encountered with "
                    //     "value %s stack top %s\n",
                    //     current_token.line, current_token.name,
                    //     current_token.lexeme_value ? current_token.lexeme_value
                    //                                : "",
                    //     nonTerminals[current_node->symbol.nT]);
                    current_token = getNextToken(fp);
                    // Push the current node back onto the stack to retry with
                    // the next token
                    stack[stack_top++] = current_node;
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
