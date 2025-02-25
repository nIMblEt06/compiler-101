/******************************************************************************
 * This file along with grammar.c handles all the operations related to grammar
 * Import this as module in parser.c/parser.h
 * 
*******************************************************************************/
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "symbolTable.h"
#include <stdbool.h>

#define RULE_SIZE 25
#define GRAMMAR_MAX_SIZE 101
#define HASH_TABLE_SIZE 101

typedef enum {program,mainFunction,otherFunctions,function,input_par,output_par,parameter_list,dataType,primitiveDatatype,
	constructedDatatype,remaining_list,stmts,typeDefinitions,typeDefinition,fieldDefinitions,fieldDefinition,moreFields,
	declarations,declaration,global_or_not,otherStmts,stmt,assignmentStmt,singleOrRecId,funCallStmt,outputParameters,
	inputParameters,iterativeStmt,conditionalStmt,ioStmt,arithmeticExpression,   op   ,booleanExpression,var,logicalOp,
	relationalOp,returnStmt,optionalReturn,idList,more_ids,definetypestmt,A} Non_terminal;
typedef enum {
		TK_ASSIGNOP, TK_COMMENT,   TK_FIELDID, TK_ID,       TK_NUM,
		TK_RNUM,     TK_FUNID,     TK_RUID,    TK_WITH,     TK_PARAMETERS,
		TK_END,      TK_WHILE,     TK_UNION,   TK_ENDUNION, TK_DEFINETYPE,
		TK_AS,       TK_TYPE,      TK_MAIN,    TK_GLOBAL,   TK_PARAMETER,
		TK_LIST,     TK_SQL,       TK_SQR,     TK_INPUT,    TK_OUTPUT,
		TK_INT,      TK_REAL,      TK_COMMA,   TK_SEM,      TK_COLON,
		TK_DOT,      TK_ENDWHILE,  TK_OP,      TK_CL,       TK_IF,
		TK_THEN,     TK_ENDIF,     TK_READ,    TK_WRITE,    TK_RETURN,
		TK_PLUS,     TK_MINUS,     TK_MUL,     TK_DIV,      TK_CALL,
		TK_RECORD,   TK_ENDRECORD, TK_ELSE,    TK_AND,      TK_OR,
		TK_NOT,      TK_LT,        TK_LE,      TK_EQ,       TK_GT,
		TK_GE,       TK_NE,		   DOLLAR} Terminal;
// NOTE: Added token DOLLAR;


/*Stores the symbols for production rule with bool to differentiate b/w terminal and non_terminal
	can use Terminal and Non_terminal arrays to get the token using the enum*/
		
typedef struct symbol{
	bool isTerminal;
	Terminal t;
	Non_terminal nT;
} sym;

//Structure for a single production rule
typedef struct Rule{
	sym lhs;
	sym rhs[RULE_SIZE];
	int rule_number;
}RULE;

// Array of all production rules
RULE Grammar[GRAMMAR_MAX_SIZE];

// Hash Table Structure for mapping a Non_terminal to its corresponding rule.
typedef struct HashTableEntry{
	Non_terminal tkn;
	int rule_indices[GRAMMAR_MAX_SIZE];
	int count;
} HashTableEntry;

HashTableEntry HashTableLHS[HASH_TABLE_SIZE];
HashTableEntry HashTableRHS[HASH_TABLE_SIZE*2];

int hashFunction(Non_terminal nt);

// function to enter rule into grammar hash table
void addRuleToHashTable(int rule_index);

// Function to initialize the hash table
void initializeHashTable();

// returns the set of Rules for a given Non_terminal lhs of the grammar rule.
HashTableEntry getRulesByLHS(Non_terminal lhs);

// this rule adds rules of grammar to the Grammar Array
void fill_grammar();
#endif
