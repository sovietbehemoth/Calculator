#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>
#include <math.h>

#include "Macro.h"
#include "Throw.h"

#define LEFT 0           //Left parenthesis token.
#define RIGHT 1          //Right parenthesis token.

#define UNDEFINED -1     //Used as terminator in tree.  
#define OBJECT 0         //Parenthesized expression.
#define VALUE 1          //Integer value (double by default).
#define CNUMBER 2        //Char value (used in parser 1).
#define OPERATOR 3       //Operator (ex: +, -, /, *).
#define IDENTIFIER 4     //String identifier.
#define VALUE_FLOAT 5    //Floating point number.

//Quick macros for sizes of AST variations.
#define AST_DSIZE sizeof(AST_t)
#define ASTR_DSIZE sizeof(ASTR_t)

//For debugging functions that won't be used in a regular build.
#define _Debug __attribute__((unused))

typedef struct AST AST_t; 
typedef struct ASTR ASTR_t;
typedef struct Match match_t;
typedef struct Token token_t;

//For first pass of parser.
typedef struct AST {
    int type;       //Defines type of node (character, operator, etc.).
    char c;         //Stores the literal char value (not needed after second parser iteration).

    AST_t *object;  //Stores an object if typed value is object.
    AST_t *next;    //Stores next AST node in tree.
} AST_t;

//For second pass of parser and also visitor.
typedef struct ASTR {
    int type;       //Defines type of node (character, operator, etc.).

    char *id;       //Stores an identifier (a word value not recognized as reserved or a number by parser).
    double value;   //Stores typed value of node (either an operator or value).

    ASTR_t *object;  //Stores an object if typed value is object.
    ASTR_t *next;    //Stores next AST node in tree.
} ASTR_t;

match_t *lex_parenthesis(
    char *input
    );

unsigned int find_pair(
    int index, 
    match_t *matches
    ); 

AST_t *lexer(
    char *input, 
    match_t *matches, 
    int offset
    );

AST_t *ast_index(
    AST_t *ast, 
    int index, 
    int n
    );

ASTR_t *astr_index(
    ASTR_t *ast, 
    int index, 
    int n
    );

ASTR_t *parser(
    AST_t *ast
    );

double calculator(
    char *expr
    );

int ast_append(
    ASTR_t *ast, 
    int type, 
    double value
    );

double visitor(
    ASTR_t *ast, 
    bool recur
    );