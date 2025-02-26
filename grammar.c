/******************************************************************************
 * This file along with grammar.h handles all the operations related to grammar
 * Import this as module in parser.c/parser.h
 * 
*******************************************************************************/
#include "grammar.h"
#define MAX_BUFF 256
int rule_cnt=0;

void fill_grammar(){
	FILE* fop = fopen("grammar.txt","r");
	if(!fop){
		printf("Error opening file;\n\n");
		return;
	}
	char buffer[MAX_BUFF];
	while(fgets(buffer,MAX_BUFF,fop) && rule_cnt < GRAMMAR_MAX_SIZE){
		char* token = strtok(buffer," .\n");
		if(!token) continue;
		
		Grammar[rule_cnt].lhs.isTerminal = false;
		Non_terminal nt = get_non_terminal(token);
		if(nt == -1) {
			printf("Invalid non-terminal in LHS: %s\n", token);
			continue;
		}
		Grammar[rule_cnt].lhs.nT = nt;
		
		int rhs_cnt = 0;
		while((token=strtok(NULL," .\n"))!=NULL && rhs_cnt < RULE_SIZE){
			if((strcmp(token,"EPSILON"))==0){
				Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = true;
				Grammar[rule_cnt].rhs[rhs_cnt].t = EPSILON;
			}else{
				int nonterm = get_non_terminal(token);
				int term = get_terminal(token);
				
				if(term!=-1){
					Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = true;
					Grammar[rule_cnt].rhs[rhs_cnt].t = term;
				}else if(nonterm!=-1){
					Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = false;
					Grammar[rule_cnt].rhs[rhs_cnt].t = nonterm;
				}
				else{
					printf("No such symbol %s \n\n",token);
					continue;  // Skip this invalid token
				}
			}
			rhs_cnt++; 
		}
		Grammar[rule_cnt].rhs_count = rhs_cnt;
		rule_cnt++;
	}
	fclose(fop);
}

Non_terminal get_non_terminal(char *str){
	// Count the number of non-terminals (54 as defined in MAX_NON_TERMINALS)
	for (int i = 0; i < MAX_NON_TERMINALS; i++) {
		if (nonTerminals[i] != NULL && strcmp(str, nonTerminals[i]) == 0){
			return (Non_terminal)i;
		}
	}
	return -1; // Not found
}

Terminal get_terminal(char *str){
	// Count the number of terminals (61 as defined in MAX_TERMINALS)
	for (int i = 0; i < MAX_TERMINALS; i++) {
		if (Terminals[i] != NULL && strcmp(str, Terminals[i]) == 0) {
			return (Terminal)i;
		}
	}
	return -1; // Not found
}

void print_grammar() {
    for (int i = 0; i < rule_cnt; i++) {
        printf("%d: %s -> ", i, nonTerminals[Grammar[i].lhs.nT]);
        int rhs_cnt = Grammar[i].rhs_count;
        
        for (int j = 0; j < rhs_cnt; j++) {
            if (Grammar[i].rhs[j].isTerminal) {
                printf("%s ", Terminals[Grammar[i].rhs[j].t]);
            } else {
                printf("%s ", nonTerminals[Grammar[i].rhs[j].t]);
            }
        }
        printf("\n");
    }
}
int main(){
	fill_grammar();
	// printf("%d",rule_cnt);
	// print_grammar();
	return 0;
}
