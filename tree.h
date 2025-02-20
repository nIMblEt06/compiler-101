#include "lexerDef.h"

#define max_sym 30

typedef struct tree *node;
typedef struct tree{
	char symbol[max_sym];
	node left;
	node right;
	token tok;
	int line_num;
};

