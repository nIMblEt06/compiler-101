#ifndef parser_def
#define parser_def

#include "lexerDef.h"

#define set_size 20
#define nT_count 53
#define max_sym 30

typedef struct fnf_sets{
	char* nT;
	char* first_set[set_size];
	char* follow_set[set_size];
} fnf_sets;

typedef struct tree *node;
typedef struct tree{
	char symbol[max_sym];
	node left;
	node right;
	token tok;
	int line_num;
};

static fnf_sets fnf[nT_count];

#endif