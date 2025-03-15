/* -----------------------------------------
             Group Number- 6
ID 2021B5A71704P Name Arushi Gulati
ID 2021B3A71260P Name Aryan Rajkumar Keshri
ID 2021B4A70887P Name Chinmay Pushkar
ID 2021B3A71102P Name Om Kotadiya Jain
ID 2021B3A71117P Name Riddhi Agarwal
ID 2021B5A70923P Name Shwetabh Niket
--------------------------------------------- */

#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

parse_tree create_node(sym symbol) {
    parse_tree node = (parse_tree)malloc(sizeof(tree_node));
    if (!node) {
        printf("Error: Memory allocation failed for parse tree node\n");
        exit(1);
    }
    node->symbol = symbol;
    node->rule_number = -1;
    node->num_children = 0;
    node->parent = NULL;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void add_child(parse_tree parent, parse_tree child) {
    if (parent->num_children >= MAX_CHILDREN) {
        printf("Error: Cannot add more children to node\n");
        return;
    }
    parent->children[parent->num_children++] = child;
    child->parent = parent;
}

void print_parse_tree(parse_tree root, int level) {
    if (!root) return;

    // Print indentation
    for (int i = 0; i < level; i++) printf("  ");

    // Print node information
    if (root->symbol.isTerminal) {
        printf("%s", Terminals[root->symbol.t]);
        if (root->tok.lexeme_value) {
            printf(" (%s)", root->tok.lexeme_value);
        }
    } else {
        printf("%s", nonTerminals[root->symbol.nT]);
        if (root->rule_number >= 0) {
            printf(" [Rule %d]", root->rule_number);
        }
    }
    printf("\n");

    for (int i = 0; i < root->num_children; i++) {
        print_parse_tree(root->children[i], level + 1);
    }
}

void free_parse_tree(parse_tree root) {
    if (!root) return;
    
    for (int i = 0; i < root->num_children; i++) {
        free_parse_tree(root->children[i]);
    }
    free(root);
}

// Helper function for inorder traversal
static void inorder_traversal(parse_tree node, FILE *fp) {
    if (!node) return;
    
    // Process left children (if any)
    if (node->num_children > 0) {
        inorder_traversal(node->children[0], fp);
    }
    
    // Process current node
    char lexeme[50] = "----";  // Default for non-leaf nodes
    char token_name[50] = "";
    char value[50] = "----";   // Default for non-numeric values
    char parent_symbol[50] = "";
    char is_leaf[5] = "";
    char node_symbol[50] = "";
    int line_no = 0;
    
    // Get parent symbol
    if (node->parent) {
        strcpy(parent_symbol, nonTerminals[node->parent->symbol.nT]);
    } else {
        strcpy(parent_symbol, "ROOT");
    }
    
    // Determine if node is a leaf
    if (node->num_children == 0) {
        strcpy(is_leaf, "yes");
        
        // For leaf nodes, get the lexeme
        if (node->tok.lexeme_value) {
            strcpy(lexeme, node->tok.lexeme_value);
        }
        
        // Get token name
        strcpy(token_name, Terminals[node->symbol.t]);
        
        // For numeric values
        if (node->symbol.t == TK_NUM) {
            sprintf(value, "%d", node->tok.num);
        } else if (node->symbol.t == TK_RNUM) {
            sprintf(value, "%f", node->tok.rnum);
        }
        
        // Get line number
        line_no = node->tok.line;
    } else {
        strcpy(is_leaf, "no");
        strcpy(node_symbol, nonTerminals[node->symbol.nT]);
    }
    
    // Print to file with proper formatting
    fprintf(fp, "%-20s %-5d %-15s %-15s %-20s %-5s %-20s\n", 
            lexeme, line_no, token_name, value, parent_symbol, is_leaf, node_symbol);
    
    // Process remaining children
    for (int i = 1; i < node->num_children; i++) {
        inorder_traversal(node->children[i], fp);
    }
}

void printParseTree(parse_tree PT, char *outfile) {
    if (!PT) return;
    
    FILE *fp = fopen(outfile, "w");
    if (!fp) {
        printf("Error: Unable to open file %s for writing\n", outfile);
        return;
    }
    
    // Print header
    fprintf(fp, "%-20s %-5s %-15s %-15s %-20s %-5s %-20s\n", 
            "lexeme", "line", "tokenName", "valueIfNumber", "parentNodeSymbol", "isLeaf", "NodeSymbol");
    fprintf(fp, "%-20s %-5s %-15s %-15s %-20s %-5s %-20s\n", 
            "--------------------", "-----", "---------------", "---------------", "--------------------", "-----", "--------------------");
    
    // Start inorder traversal
    inorder_traversal(PT, fp);
    
    fclose(fp);
} 