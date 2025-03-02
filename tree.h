#ifndef TREE_H
#define TREE_H

#include "grammar.h"
#include "lexer.h"

#define MAX_CHILDREN 25  // Same as RULE_SIZE

typedef struct tree_node {
	sym symbol;          // Grammar symbol (terminal or non-terminal)
	token tok;           // Token info if terminal
	int rule_number;     // Grammar rule number used for expansion
	int num_children;    // Number of children nodes
	struct tree_node* children[MAX_CHILDREN];  // Array of child nodes
	struct tree_node* parent;    // Parent node
} tree_node;

typedef tree_node* parse_tree;

// Function declarations
parse_tree create_node(sym symbol);
void add_child(parse_tree parent, parse_tree child);
void print_parse_tree(parse_tree root, int level);
void free_parse_tree(parse_tree root);

#endif

