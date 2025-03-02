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

    // Print children recursively
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