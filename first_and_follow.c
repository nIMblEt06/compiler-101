/******************************************************************************
 * This file along with first_and_follow.h handles all the operations related to first_and_follow
 * Import this as module in parser.c/parser.h
*******************************************************************************/
#include "first_and_follow.h"
#include "grammar.h"

/*
* Why have we added the hash function?
We are usign the  hash function to be able to find out the GRAMMAR rules containing the
non-terminal in their LHS so that we can find out their FIRST() and FOLLOW(). 
*/
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


// Assuming we already have the grammar rules. 
set *first_set(sym x){
    // sym x -> Non terminal whose first(x) we want to find. 

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
    
}


