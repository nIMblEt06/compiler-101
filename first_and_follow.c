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
	// Store the rule index at the current count position, then increment count
	if (HashTableLHS[hashInd].count < GRAMMAR_MAX_SIZE) {
		HashTableLHS[hashInd].rule_indices[HashTableLHS[hashInd].count] = rule_index;
		HashTableLHS[hashInd].count++;
	}
}

void addRulesToHashTableRHS(int rule_index){
    sym *rhs_list = Grammar[rule_index].rhs;
    
    for (int i = 0; i < RULE_SIZE && i < Grammar[rule_index].rhs_count; i++){
        if (rhs_list[i].isTerminal == true){
            continue;  // Skip terminals
        }
        
        Non_terminal rhs = rhs_list[i].nT;
        int hashInd = hashFunction(rhs);
        
        if(HashTableRHS[hashInd].count == 0){
            HashTableRHS[hashInd].tkn = rhs; // if hashInd is empty
        }
        
        // Store the rule index at the current count position, then increment count
        if (HashTableRHS[hashInd].count < GRAMMAR_MAX_SIZE) {
            HashTableRHS[hashInd].rule_indices[HashTableRHS[hashInd].count] = rule_index;
            HashTableRHS[hashInd].count++;
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
    if(!x.isTerminal) {
        int ind = hashFunction(x.nT);
        return first_and_follow_table + ind;
    }
}

void fill_grammar() {
    printf("Initializing hash tables...\n");
    initializeHashTable(); // Initialize hash tables before filling

    printf("Opening grammar.txt...\n");
    FILE* fop = fopen("grammar.txt","r");
    if (!fop) {
        printf("Error opening file;\n\n");
        return;
    }

    rule_cnt = 0;  // Initialize rule_cnt
    char buffer[MAX_BUFF];

    while (fgets(buffer, MAX_BUFF, fop) && rule_cnt < GRAMMAR_MAX_SIZE) {
        
        char* token = strtok(buffer, " .\n");
        if (!token) continue;
        // Handle LHS
        Grammar[rule_cnt].lhs.isTerminal = false;
        Non_terminal nt = get_non_terminal(token);
        if (nt == -1) {
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
set *first_set(sym x) {
    // Create a static array to track visited non-terminals and prevent infinite recursion
    static bool visited[MAX_NON_TERMINALS] = {false};
    
    set *list_first = create_set();
    if (!list_first) {
        printf("Error: Failed to create FIRST set\n");
        return NULL;
    }
    
    // If terminal, just add it to the set
    if (x.isTerminal) {
        add_to_set(list_first, x.t);
        return list_first;
    }
    
    // Check if we've already visited this non-terminal to prevent infinite recursion
    if (visited[x.nT]) {
        return list_first;  // Return empty set to break the recursion
    }
    
    // Mark as visited
    visited[x.nT] = true;
    
    // Get rules where x is in LHS
    HashTableEntry rules_entry = getRulesByLHS(x.nT);
    
    // For each rule with x as LHS
    for (int i = 0; i < rules_entry.count; i++) {
        int rule_ind = rules_entry.rule_indices[i];
        if (rule_ind < 0 || rule_ind >= rule_cnt) {
            continue;
        }
        
        RULE rule = Grammar[rule_ind];
        
        // Process each symbol in RHS until we find one that can't derive epsilon
        int j = 0;
        bool continue_rule = true;
        
        while (j < rule.rhs_count && continue_rule) {
            sym current_sym = rule.rhs[j];
            
            if (current_sym.isTerminal) {
                add_to_set(list_first, current_sym.t);
                continue_rule = false;
            } else {
                // Skip self-recursion to prevent infinite loops
                if (current_sym.nT == x.nT) {
                    j++;
                    continue;
                }
                
                // Recursively compute FIRST set of non-terminal
                set *first_of_sym = first_set(current_sym);
                if (!first_of_sym) {
                    j++;
                    continue;
                }
                
                // Add all non-epsilon terminals to our set
                for (int k = 0; k < first_of_sym->size; k++) {
                    if (first_of_sym->t[k] != EPSILON) {
                        add_to_set(list_first, first_of_sym->t[k]);
                    }
                }
                
                // If this symbol can't derive epsilon, stop processing this rule
                if (!first_of_sym->containsEpsilon) {
                    continue_rule = false;
                }
                
                free(first_of_sym);
            }
            j++;
        }
        
        // If we processed all symbols and they all could derive epsilon,
        // add epsilon to our set
        if (continue_rule && j == rule.rhs_count) {
            add_to_set(list_first, EPSILON);
            list_first->containsEpsilon = true;
        }
    }
    
    // Unmark as visited before returning
    visited[x.nT] = false;
    
    return list_first;
}

set *follow_set(sym x) {
    if (!x.isTerminal) {
        return follow_set_util(x.nT);
    }
    return NULL;
}

set *follow_set_util(Non_terminal x) {
    // Create a static array to track visited non-terminals and prevent infinite recursion
    static bool visited[MAX_NON_TERMINALS] = {false};
    
    set *follow_right = create_set();
    if (!follow_right) {
        printf("Error: Failed to create FOLLOW set\n");
        return NULL;
    }
    
    // Add TK_DOLLAR to FOLLOW set of start symbol (program)
    if (x == program) {
        add_to_set(follow_right, TK_DOLLAR);
    }
    
    // Check if we've already visited this non-terminal to prevent infinite recursion
    if (visited[x]) {
        return follow_right;  // Return empty set to break the recursion
    }
    
    // Mark as visited
    visited[x] = true;
    
    HashTableEntry hash_entry_rhs = getRulesByRHS(x);
    
    // For each rule where x appears in RHS
    for (int i = 0; i < hash_entry_rhs.count; i++) {
        int rule_index = hash_entry_rhs.rule_indices[i];
        if (rule_index < 0 || rule_index >= rule_cnt) {
            continue;
        }
        
        RULE rule = Grammar[rule_index];
        
        // Find x in RHS
        for (int j = 0; j < rule.rhs_count; j++) {
            if (!rule.rhs[j].isTerminal && rule.rhs[j].nT == x) {
                // If x is the last symbol, add FOLLOW(LHS)
                if (j == rule.rhs_count - 1) {
                    sym lhs_sym = {.isTerminal = false, .nT = rule.lhs.nT};
                    // Avoid infinite recursion - don't compute FOLLOW of same non-terminal
                    if (lhs_sym.nT != x) {
                        set *follow_lhs = follow_set(lhs_sym);
                        if (follow_lhs) {
                            for (int k = 0; k < follow_lhs->size; k++) {
                                add_to_set(follow_right, follow_lhs->t[k]);
                            }
                            free(follow_lhs);
                        }
                    }
                } else {
                    // Get FIRST of everything that follows x in this rule
                    bool all_derive_epsilon = true;
                    for (int k = j + 1; k < rule.rhs_count && all_derive_epsilon; k++) {
                        set *first_next = NULL;
                        if (rule.rhs[k].isTerminal) {
                            first_next = create_set();
                            if (first_next) {
                                add_to_set(first_next, rule.rhs[k].t);
                                all_derive_epsilon = false;
                            }
                        } else {
                            first_next = first_set(rule.rhs[k]);
                            if (first_next) {
                                all_derive_epsilon = first_next->containsEpsilon;
                            }
                        }
                        
                        if (first_next) {
                            // Add all non-epsilon symbols to FOLLOW set
                            for (int m = 0; m < first_next->size; m++) {
                                if (first_next->t[m] != EPSILON) {
                                    add_to_set(follow_right, first_next->t[m]);
                                }
                            }
                            free(first_next);
                        }
                    }
                    
                    // If everything after x can derive epsilon, add FOLLOW(LHS)
                    if (all_derive_epsilon) {
                        sym lhs_sym = {.isTerminal = false, .nT = rule.lhs.nT};
                        // Avoid infinite recursion
                        if (lhs_sym.nT != x) {
                            set *follow_lhs = follow_set(lhs_sym);
                            if (follow_lhs) {
                                for (int k = 0; k < follow_lhs->size; k++) {
                                    add_to_set(follow_right, follow_lhs->t[k]);
                                }
                                free(follow_lhs);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Unmark as visited before returning
    visited[x] = false;
    
    return follow_right;
}

// Set operations
set* create_set() {
    set* s = (set*)malloc(sizeof(set));
    if (!s) {
        printf("Error: Failed to allocate memory for set\n");
        return NULL;
    }
    s->size = 0;
    s->containsEpsilon = false;
    return s;
}

void add_to_set(set* s, Terminal t) {
    if (!s) return;
    
    // Check if terminal is already in set
    for (int i = 0; i < s->size; i++) {
        if (s->t[i] == t) return;
    }
    
    // Add terminal if set is not full
    if (s->size < max_terminal) {
        s->t[s->size++] = t;
        if (t == EPSILON) {
            s->containsEpsilon = true;
        }
    } else {
        printf("Error: Set is full, cannot add more terminals\n");
    }
}





