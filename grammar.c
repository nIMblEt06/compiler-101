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
	while(fgets(buffer,MAX_BUFF,fop)){
		char* token = strtok(buffer," .\n");
		if(!token) continue;
		Grammar[rule_cnt].lhs.isTerminal = false;
		Grammar[rule_cnt].lhs.nT = get_non_terminal(token);
		//printf("lhs token: %s\n",token);
		int rhs_cnt = 0;
		while((token=strtok(NULL," .\n"))!=NULL){
			size_t length = strlen(token);
			if((strcmp(token,"EPSILON"))==0){
				Grammar[rule_cnt].rhs[rhs_cnt].isTerminal= true;
				Grammar[rule_cnt].rhs[rhs_cnt].t = EPSILON;
				//printf("In EPS %d: %s\n",rule,token);
			}else{
				int nonterm = get_non_terminal(token);
				int term = get_terminal(token);
				//printf("term: %d nonterm: %d , %s, %zu\n",term,nonterm,token,length);
				if(term!=-1){
					Grammar[rule_cnt].rhs[rhs_cnt].isTerminal=true;
					Grammar[rule_cnt].rhs[rhs_cnt].t = term;
					//printf("%d in term %d: %s\n",term,rule,token);
				}else if(nonterm!=-1){
					Grammar[rule_cnt].rhs[rhs_cnt].isTerminal=false;
					Grammar[rule_cnt].rhs[rhs_cnt].t = nonterm;
					//printf("%d in non term %d: %s\n",nonterm,rule,token);
				}
				else{
					printf("No such symbol %s \n\n",token);
				}
			}
			rhs_cnt++; 
		}
		Grammar[rule_cnt].rhs_count=rhs_cnt;
		rule_cnt++;
	}
	fclose(fop);
}

Non_terminal get_non_terminal(char *str){
	for (int i = 0; nonTerminals[i] != NULL; i++) {
        if (strcmp(str, nonTerminals[i]) == 0){
			//printf("Matched in NonT  %s\n",str);
			return (Non_terminal)i;
		}
    }
    return -1; // Not found
}
Terminal get_terminal(char *str){
	for (int i = 0; Terminals[i] != NULL; i++) {
        if (strcmp(str, Terminals[i]) == 0) {
			//printf("Matched in T %s\n",str);
			return (Terminal)i;
		}
    }
    return -1; // Not found
}

void print_grammar() {
    for (int i = 0; i < rule_cnt; i++) {
        // Add bounds checking for Grammar array
        if (Grammar[i].lhs.nT < 0 || Grammar[i].lhs.nT >= MAX_NON_TERMINALS) {
            printf("Error: Invalid non-terminal index %d in rule %d\n", Grammar[i].lhs.nT, i);
            continue;
        }
        
        printf("%d: %s -> ", i, nonTerminals[Grammar[i].lhs.nT]);
        int rhs_cnt = Grammar[i].rhs_count;
        
        for (int j = 0; j < rhs_cnt; j++) {
            if (Grammar[i].rhs[j].isTerminal) {
                // Add bounds checking for terminals
                if (Grammar[i].rhs[j].t < 0 || Grammar[i].rhs[j].t >= MAX_TERMINALS) {
                    printf("Error: Invalid terminal index %d in rule %d\n", Grammar[i].rhs[j].t, i);
                    continue;
                }
                printf("%s ", Terminals[Grammar[i].rhs[j].t]);
            } else {
                // Add bounds checking for non-terminals
                if (Grammar[i].rhs[j].t < 0 || Grammar[i].rhs[j].t >= MAX_NON_TERMINALS) {
                    printf("Error: Invalid non-terminal index %d in rule %d\n", Grammar[i].rhs[j].t, i);
                    continue;
                }
                printf("%s ", nonTerminals[Grammar[i].rhs[j].t]);
            }
        }
        printf("\n");
    }
}
int main(){
	fill_grammar();
	// printf("%d",rule_cnt);
	print_grammar();
	return 0;
}
