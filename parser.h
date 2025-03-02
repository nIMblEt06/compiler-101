#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "symbolTable.h"
#include "parserDef.h"
#include "parse_table.h"
#include "tree.h"

// Parser functions
parse_tree create_parse_tree(FILE* fp);
void print_parse_tree(parse_tree root, int level);
void free_parse_tree(parse_tree root);

// Error handling
void handle_parse_error(token current_token, Non_terminal current_nt);

#endif