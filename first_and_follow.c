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

void addRuleToHashTable(int rule_index){
	Non_terminal lhs = Grammar[rule_index].lhs.nT;
	int hashInd = hashFunction(lhs);
	if(HashTable[hashInd].count == 0){
		HashTable[hashInd].lhs = lhs; // if hashIndex is empty
	}
	HashTable[hashInd].count += 1;
    HashTable[hashInd].rule_indices[HashTable[hashInd].count++] = rule_index;
}

void initializeHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashTable[i].count = 0;
    }
}

// returns the set of Rules for a given Non_terminal lhs of the grammar rule.
HashTableEntry getRulesByLHS(Non_terminal lhs){
    int ind = hashFunction(lhs);
    HashTableEntry rules_entry = HashTable[ind];
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
        for(int i = 0; i < count; i++){ // iterating through all the rules containing x in LHS
            int rule_ind = rules_entry.rule_indices[i];
            RULE rule = Grammar[rule_ind];
            if (rule.rhs[0].isTerminal == true){ // if first of RHS is a terminal
                list_first->t[list_first->size] = rule.rhs[0].t;
                list_first->size += 1;
            } else if (rule.rhs[0].isTerminal == false){ // if first of RHS is a non-terminal
                int j = 0;
                while(j < max_terminal && rule.rhs[j].isTerminal == false){
                    set *rhs_first = first_set(rule.rhs[j]);
                    // add elements of rhs_first to list_first;
                    //
                    //
                    if (rhs_first->containsEpsilon == false){
                        break;
                    } else {
                        j++; // so that we can find the FIRST(next_non_terminal)
                    }
                }
            }
        }
    }

    return list_first;
}

set *follow_set(sym x){
    
}


