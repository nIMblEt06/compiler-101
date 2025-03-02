/******************************************************************************
 * This file along with first_and_follow.h handles all the operations related to first_and_follow
 * Import this as module in parser.c/parser.h
*******************************************************************************/
#include "first_and_follow.h"
#include "grammar.h"

/******** 
* Why have we added the hash function?

* We are usign the  hash function to be able to find out the GRAMMAR rules containing the
* non-terminal in their LHS so that we can find out their FIRST() and FOLLOW(). 
*******/

int hashFunction(Non_terminal nt){
	return (int)nt % HASH_TABLE_SIZE;
}

void addRuleToHashTableLHS(int rule_index){
	Non_terminal lhs = Grammar[rule_index].lhs.nT;
	int hashInd = hashFunction(lhs);
	if(HashTableLHS[hashInd].count == 0){
		HashTableLHS[hashInd].tkn = lhs; // if hashIndex is empty
	}
	HashTableLHS[hashInd].count += 1;
    HashTableLHS[hashInd].rule_indices[HashTableLHS[hashInd].count++] = rule_index;
}

void addRulesToHashTableRHS(int rule_index){
    sym *rhs_list = Grammar[rule_index].rhs;
    
    for (int i = 0;i < RULE_SIZE; i++){
        if (rhs_list[i].isTerminal == true || (rhs_list[i].nT == '$' /*some End Of String char delimiter*/)){
            break;
        } else {
            // if (rhs_list[i].isTerminal == false)
            Non_terminal rhs = rhs_list[i].nT;
            int hashInd = hashFunction(rhs);
            if(HashTableRHS[hashInd].count == 0){
                HashTableRHS[hashInd].tkn = rhs; // if hashInd is empty
            }

            HashTableRHS[hashInd].count += 1;
            HashTableRHS[hashInd].rule_indices[HashTableRHS[hashInd].count++] = rule_index;
        }
    }
}

void initializeHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashTableLHS[i].count = 0;
        HashTableRHS[i].count = 0;
    }
}

// returns the set of Rules for a given Non_terminal lhs of the grammar rule.
HashTableEntry getRulesByLHS(Non_terminal lhs){
    int ind = hashFunction(lhs);
    HashTableEntry rules_entry = HashTableLHS[ind];
    return rules_entry;
}

HashTableEntry getRulesByRHS(Non_terminal rhs){
    int ind = hashFunction(rhs);
    HashTableEntry rules_entry = HashTableRHS[ind];
    return rules_entry;
}

// uses the hashFunction to index and get first_and_follow entry from the first_and_follow_table from the sym of token.
FIRST_AND_FOLLOW_ENTRY *get_first_and_follow_entry(sym x){
    if(x.isTerminal == true) printf("FIRST_AND_FOLLOW does not exist for terminal_tokens.\n");
    else {
        int ind = hashFunction(x.nT);
        return first_and_follow_table + ind;
    }
}


void fill_grammar() {
    initializeHashTable(); // Initialize hash tables before filling

    FILE* fop = fopen("grammar.txt", "r");
    if (!fop) {
        printf("Error opening file;\n\n");
        return;
    }

    int rule_cnt = 0;
    char buffer[MAX_BUFF];
    
    while (fgets(buffer, MAX_BUFF, fop) && rule_cnt < GRAMMAR_MAX_SIZE) {
        char* token = strtok(buffer, " .\n");
        if (!token) continue;
        
        // Handle LHS
        Grammar[rule_cnt].lhs.isTerminal = false;
        Non_terminal nt = get_non_terminal(token);
        if (nt == -1) {
            printf("Invalid non-terminal in LHS: %s\n", token);
            continue;
        }
        Grammar[rule_cnt].lhs.nT = nt;
        
        // Handle RHS
        int rhs_cnt = 0;
        while ((token = strtok(NULL, " .\n")) != NULL && rhs_cnt < RULE_SIZE) {
            if ((strcmp(token, "EPSILON")) == 0) {
                Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = true;
                Grammar[rule_cnt].rhs[rhs_cnt].t = EPSILON;
            } else {
                int nonterm = get_non_terminal(token);
                int term = get_terminal(token);
                
                if (term != -1) {
                    Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = true;
                    Grammar[rule_cnt].rhs[rhs_cnt].t = term;
                } else if (nonterm != -1) {
                    Grammar[rule_cnt].rhs[rhs_cnt].isTerminal = false;
                    Grammar[rule_cnt].rhs[rhs_cnt].nT = nonterm;  // Fixed: use nT instead of t for non-terminals
                } else {
                    printf("No such symbol %s \n\n", token);
                    continue;
                }
            }
            rhs_cnt++;
        }
        Grammar[rule_cnt].rhs_count = rhs_cnt;
        
        // Add rule to both hash tables
        addRuleToHashTableLHS(rule_cnt);
        addRulesToHashTableRHS(rule_cnt);
        
        rule_cnt++;
    }
    fclose(fop);
}

// TO-DO:
// -> Optimize first_and_follow_set using Memoisation.

// Assuming we already have the grammar rules. 
set *first_set(sym x){
    // TO-DO
    // -> Optimize using memoization.
    set *list_first = (set *) malloc(sizeof(set));
    list_first->size = 0;
    if (x.isTerminal == true){
        list_first->t[list_first->size] = x.t;
        list_first->size += 1;
    } else {
        HashTableEntry rules_entry = getRulesByLHS(x.nT);
        int count = rules_entry.count;
        for(int i = 0; i < count; i++){ 
            // iterating through all the rules containing x in LHS
            int rule_ind = rules_entry.rule_indices[i];
            RULE rule = Grammar[rule_ind];
            
            if (rule.rhs[0].isTerminal == true){ 
                // if first of RHS is a terminal
                list_first->t[list_first->size] = rule.rhs[0].t;
                list_first->size += 1;
            } else if (rule.rhs[0].isTerminal == false){ 
                // if first of RHS is a non-terminal
                int j = 0;
                while(j < max_terminal){
                    if(rule.rhs[j].isTerminal == true){
                        list_first->t[list_first->size] = rule.rhs[j].t;
                    } else {
                        set *rhs_first = first_set(rule.rhs[j]);
                        
                        // add element to the list.
                        for (int k = 0; k < rhs_first->size; k++){
                            list_first->t[list_first->size] = rhs_first->t[k];
                            list_first->size += 1;
                        }
                        if (rhs_first->containsEpsilon == false){
                            break;
                        } else {
                            j++; // so that we can find the FIRST(next_non_terminal)
                        }
                    }
                }
            }
        }
    }
    return list_first;
}

set *follow_set(sym x){
    set *follow_right = NULL;
    if (x.isTerminal == true){
        printf("FOLLOW_SET does not exist for TERMINAL_TOKENS.\n");
    } else {
        set *follow_right = follow_set_util(x.nT);
    }
    return follow_right;
}

// Used a follow_set_util as seperate function, cause we practically need to find the FOLLOW(Non_terminal) token wise.
set *follow_set_util(Non_terminal x){
    set *follow_right = (set *) malloc(sizeof(set)*max_terminal);
    HashTableEntry hash_entry_rhs = getRulesByRHS(x);

    int count = hash_entry_rhs.count;
    for(int i = 0; i < count; i++){
        int rule_index = hash_entry_rhs.rule_indices[i];
        RULE rule = Grammar[rule_index];
        // Assuming the RULE_SIZE constant accounts for 1 End-Of-Rule character.

        for(int j = 0;j < RULE_SIZE; j++){  
            if (rule.rhs[j].isTerminal == false && rule.rhs[j].nT == x){
                // if current token is our input target token.
                if(j + 1 < RULE_SIZE && rule.rhs[j+1].isTerminal==true){
                    if (rule.rhs[j+1].t == $){
                        // the token next to our token is EOL char.
                        // therefore, taking follow(LHS)
                        set *follow = follow_set(rule.lhs);
                        for(int k =0; k < follow->size; k++){
                            follow_right->t[follow_right->size] = follow->t[k];
                        }
                    } else {
                        // the token next to my token is a terminal token.
                        set *first_rhs = first_set(rule.rhs[j+1]);
                        for(int k =0; k < first_rhs->size; k++){
                            follow_right->t[follow_right->size] = first_rhs->t[k];
                        }
                        if (first_rhs->containsEpsilon == false){
                            break;
                        } else {
                            continue;
                        }
                    }
                } else { 
                    // if the token is non terminal
                    set *first_rhs = first_set(rule.rhs[j+1]);
                    for(int k =0; k < first_rhs->size; k++){
                        follow_right->t[follow_right->size] = first_rhs->t[k];
                    }
                    if (first_rhs->containsEpsilon == false){
                        break;
                    } else {
                        continue;
                    }
                    
                }
            } else continue;
        }
    }
    return follow_right;
}





