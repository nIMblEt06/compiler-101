/* -----------------------------------------
             Group Number- 6
ID 2021B5A71704P Name Arushi Gulati
ID 2021B3A71260P Name Aryan Rajkumar Keshri
ID 2021B4A70887P Name Chinmay Pushkar
ID 2021B3A71102P Name Om Kotadiya Jain
ID 2021B3A71117P Name Riddhi Agarwal
ID 2021B5A70923P Name Shwetabh Niket
--------------------------------------------- */

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
void printParseTree(parse_tree PT, char *outfile);

#endif

