/* -----------------------------------------
             Group Number- 6
ID 2021B5A71704P Name Arushi Gulati
ID 2021B3A71260P Name Aryan Rajkumar Keshri
ID 2021B4A70887P Name Chinmay Pushkar
ID 2021B3A71102P Name Om Kotadiya Jain
ID 2021B3A71117P Name Riddhi Agarwal
ID 2021B5A70923P Name Shwetabh Niket
--------------------------------------------- */

#include "symbolTable.h"

// Define the non-terminals array
const char *nonTerminals[] = {
    "program", "mainFunction", "otherFunctions", "function", "input_par",
    "output_par", "parameter_list", "dataType", "primitiveDatatype", "constructedDatatype",
    "remaining_list", "stmts", "typeDefinitions", "actualOrRedefined", "typeDefinition",
    "fieldDefinitions", "fieldDefinition", "fieldtype", "moreFields", "declarations",
    "declaration", "global_or_not", "otherStmts", "stmt", "assignmentStmt",
    "singleOrRecId", "oneExpansion", "moreExpansions", "funCallStmt",
    "outputParameters", "inputParameters", "iterativeStmt", "conditionalStmt", "elsePart",
    "ioStmt", "arithmeticExpression", "expPrime", "term", "termPrime",
    "factor", "highPrecedenceOperator", "lowPrecedenceOperators", "booleanExpression", "var",
    "logicalOp", "relationalOp", "returnStmt", "optionalReturn", "idList",
    "more_ids", "definetypestmt", "A", "option_single_constructed", NULL
};

// Define the terminals array
const char *Terminals[] = {
    "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM",
    "TK_RNUM", "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS",
    "TK_END", "TK_WHILE", "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE",
    "TK_AS", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER",
    "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT",
    "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON",
    "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF",
    "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN",
    "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL",
    "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR",
    "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT",
    "TK_GE", "TK_NE", "TK_DOLLAR", "ERROR", "TK_DELIM",
    "EPSILON"
}; 