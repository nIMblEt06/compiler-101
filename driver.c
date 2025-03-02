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

// Test function declarations
void test_lexer(const char* test_file);
void test_grammar();
void test_first_sets();
void test_follow_sets();
void test_parse_table();
void test_parse_tree(const char* test_file);
void test_integrated_components();
void print_test_header(const char* test_name);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    print_test_header("LEXER TESTS");
    test_lexer(argv[1]);

    print_test_header("GRAMMAR TESTS");
    test_grammar();

    print_test_header("FIRST SETS TESTS");
    test_first_sets();

    print_test_header("FOLLOW SETS TESTS");
    test_follow_sets();

    print_test_header("PARSE TABLE TESTS");
    test_parse_table();

    print_test_header("PARSE TREE TESTS");
    test_parse_tree(argv[1]);

    print_test_header("INTEGRATED COMPONENT TESTS");
    test_integrated_components();

    return 0;
}

void print_test_header(const char* test_name) {
    printf("\n=================================\n");
    printf("    %s\n", test_name);
    printf("=================================\n\n");
}

void test_lexer(const char* test_file) {
    printf("Testing lexer with file: %s\n\n", test_file);
    
    FILE* fp = fopen(test_file, "r");
    if (!fp) {
        printf("Error: Could not open test file\n");
        return;
    }
    
    // Initialize lexer and print tokens
    token token_val;
    while (1) {
        token_val = getNextToken(fp);
        if (strcmp(token_val.name, "EOF") == 0) break;
        
        printf("Token: %-15s Line: %-4d Value: %s\n", 
               token_val.name, 
               token_val.line,
               token_val.lexeme_value);
    }
    
    fclose(fp);
}

void test_grammar() {
    printf("Testing grammar initialization...\n\n");
    
    // Initialize grammar
    initializeHashTable();
    fill_grammar();
    
    // Print grammar rules
    printf("Grammar Rules:\n");
    for (int i = 0; i < rule_cnt; i++) {
        RULE rule = Grammar[i];
        printf("Rule %d: %s -> ", i, nonTerminals[rule.lhs.nT]);
        for (int j = 0; j < rule.rhs_count; j++) {
            if (rule.rhs[j].isTerminal) {
                printf("%s ", Terminals[rule.rhs[j].t]);
            } else {
                printf("%s ", nonTerminals[rule.rhs[j].nT]);
            }
        }
        printf("\n");
    }
    
    // Print hash table statistics
    printf("\nHash Table Statistics:\n");
    printf("Total rules: %d\n", rule_cnt);
    printf("Hash table size: %d\n", HASH_TABLE_SIZE);
}

void test_first_sets() {
    printf("Testing FIRST set computation...\n\n");
    
    // Test cases for important non-terminals
    Non_terminal test_nts[] = {
        program,
        mainFunction,
        stmts,
        stmt,
        declaration,
        arithmeticExpression,
        term,
        factor
    };
    
    const char* test_nt_names[] = {
        "program",
        "mainFunction",
        "stmts",
        "stmt",
        "declaration",
        "arithmeticExpression",
        "term",
        "factor"
    };
    
    for (int i = 0; i < sizeof(test_nts)/sizeof(test_nts[0]); i++) {
        sym test_sym;
        test_sym.isTerminal = false;
        test_sym.nT = test_nts[i];
        set* first = first_set(test_sym);
        
        printf("FIRST(%s): ", test_nt_names[i]);
        if (first) {
            for (int j = 0; j < first->size; j++) {
                printf("%s ", Terminals[first->t[j]]);
            }
            printf("\n");
        } else {
            printf("(empty)\n");
        }
    }
}

void test_follow_sets() {
    printf("Testing FOLLOW set computation...\n\n");
    
    // Test cases for important non-terminals
    Non_terminal test_nts[] = {
        program,
        mainFunction,
        stmts,
        stmt,
        declaration,
        arithmeticExpression,
        term,
        factor
    };
    
    const char* test_nt_names[] = {
        "program",
        "mainFunction",
        "stmts",
        "stmt",
        "declaration",
        "arithmeticExpression",
        "term",
        "factor"
    };
    
    for (int i = 0; i < sizeof(test_nts)/sizeof(test_nts[0]); i++) {
        sym test_sym;
        test_sym.isTerminal = false;
        test_sym.nT = test_nts[i];
        set* follow = follow_set(test_sym);
        
        printf("FOLLOW(%s): ", test_nt_names[i]);
        if (follow) {
            for (int j = 0; j < follow->size; j++) {
                printf("%s ", Terminals[follow->t[j]]);
            }
            printf("\n");
        } else {
            printf("(empty)\n");
        }
    }
}

void test_parse_table() {
    printf("Testing LL(1) parse table construction...\n\n");
    
    // Initialize and compute parse table
    compute_parse_table();
    
    // Check for conflicts
    report_parse_table_conflicts();
    
    // Print the parse table
    print_parse_table();
    
    // Test specific entries
    printf("\nTesting specific parse table entries:\n");
    
    // Test cases array - add important parsing cases here
    struct {
        Non_terminal nt;
        Terminal t;
        const char* nt_name;
        const char* t_name;
    } test_cases[] = {
        {program, TK_MAIN, "program", "TK_MAIN"},
        {mainFunction, TK_MAIN, "mainFunction", "TK_MAIN"},
        {stmts, TK_TYPE, "stmts", "TK_TYPE"},
        {declaration, TK_TYPE, "declaration", "TK_TYPE"},
        {arithmeticExpression, TK_ID, "arithmeticExpression", "TK_ID"},
        {term, TK_ID, "term", "TK_ID"},
        {factor, TK_ID, "factor", "TK_ID"}
    };
    
    for (int i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        parse_table_entry entry = get_parse_table_entry(test_cases[i].nt, test_cases[i].t);
        printf("Entry [%s, %s]: ", test_cases[i].nt_name, test_cases[i].t_name);
        if (entry.is_error) {
            printf("Error (no rule)\n");
        } else {
            printf("Rule %d\n", entry.rule_number);
            
            // Print the actual rule for verification
            RULE rule = Grammar[entry.rule_number];
            printf("  %s -> ", nonTerminals[rule.lhs.nT]);
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
    
    // Test error handling
    printf("\nTesting error handling:\n");
    parse_table_entry error_entry = get_parse_table_entry(program, TK_ENDIF);
    printf("Invalid entry [program, TK_ENDIF]: %s\n", 
           error_entry.is_error ? "Correctly marked as error" : "Incorrectly marked as valid");
}

void validate_parse_tree(parse_tree node) {
    if (!node) return;

    // Validate current node
    if (!node->symbol.isTerminal) {
        printf("Non-terminal: %s\n", nonTerminals[node->symbol.nT]);
        if (node->rule_number >= 0) {
            printf("  Using Rule %d: ", node->rule_number);
            RULE rule = Grammar[node->rule_number];
            printf("%s -> ", nonTerminals[rule.lhs.nT]);
            for (int i = 0; i < rule.rhs_count; i++) {
                if (rule.rhs[i].isTerminal) {
                    printf("%s ", Terminals[rule.rhs[i].t]);
                } else {
                    printf("%s ", nonTerminals[rule.rhs[i].nT]);
                }
            }
            printf("\n");
            
            // Validate number of children matches rule RHS count
            if (node->num_children != rule.rhs_count) {
                printf("  WARNING: Number of children (%d) doesn't match rule RHS count (%d)\n",
                       node->num_children, rule.rhs_count);
            }
        }
    } else {
        printf("Terminal: %s", Terminals[node->symbol.t]);
        if (node->tok.lexeme_value) {
            printf(" (Value: %s)", node->tok.lexeme_value);
        }
        printf("\n");
    }

    // Recursively validate children
    for (int i = 0; i < node->num_children; i++) {
        validate_parse_tree(node->children[i]);
    }
}


void test_parse_tree(const char* test_file) {
    printf("Testing parse tree creation with file: %s\n\n", test_file);
    
    FILE* fp = fopen(test_file, "r");
    if (!fp) {
        printf("Error: Could not open test file\n");
        return;
    }

    // Initialize grammar and parse table first
    initializeHashTable();
    fill_grammar();
    compute_parse_table();
    
    // Create and print parse tree
    parse_tree tree = create_parse_tree(fp);
    if (tree) {
        printf("\nParse Tree Structure:\n");
        printf("====================\n\n");
        print_parse_tree(tree, 0);
        
        // Validate tree structure
        printf("\nTree Validation:\n");
        printf("===============\n");
        validate_parse_tree(tree);
        
        free_parse_tree(tree);
    } else {
        printf("Failed to create parse tree\n");
    }
    
    fclose(fp);
}

void test_integrated_components() {
    printf("Testing integrated components...\n\n");
    
    // Test the interaction between grammar, first/follow sets and parse table
    printf("1. Verifying grammar rules are properly reflected in parse table:\n");
    for (int i = 0; i < rule_cnt; i++) {
        RULE rule = Grammar[i];
        sym first_sym = rule.rhs[0];
        
        // Get FIRST set of the first symbol in RHS
        set* first;
        if (first_sym.isTerminal) {
            first = create_set();
            add_to_set(first, first_sym.t);
        } else {
            first = first_set(first_sym);
        }
        
        // Check if this rule is properly reflected in parse table
        printf("Rule %d: Checking parse table entries for %s\n", 
               i, nonTerminals[rule.lhs.nT]);
        
        if (first) {
            for (int j = 0; j < first->size; j++) {
                parse_table_entry entry = get_parse_table_entry(rule.lhs.nT, first->t[j]);
                if (!entry.is_error && entry.rule_number == i) {
                    printf("  - Correctly found in table for terminal %s\n", 
                           Terminals[first->t[j]]);
                }
            }
        }
    }
    
    printf("\n2. Testing error recovery paths:\n");
    // Test some known error cases
    parse_table_entry error_cases[] = {
        get_parse_table_entry(program, TK_END),
        get_parse_table_entry(mainFunction, TK_NUM),
        get_parse_table_entry(declaration, TK_WHILE)
    };
    
    for (int i = 0; i < sizeof(error_cases)/sizeof(error_cases[0]); i++) {
        printf("Error case %d: %s\n", i + 1,
               error_cases[i].is_error ? "Correctly marked as error" : "Unexpected valid entry");
    }
}