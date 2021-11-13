//
// Created by Admin on 12/10/2021.
//



//identifikator
#include "cmake-build-debug/string.h"

#define ID          0
#define INT         1
#define FLOAT       2
#define RETEZEC      3

//klicova slova
#define DO          10
#define ELSE        11
#define END         12
#define FUNCTION    13
#define GLOBAL      14
#define IF          15
#define INTEGER     16
#define LOCAL       17
#define NIL         18
#define NUMBER      19
#define REQUIRE     20
#define RETURN      21
#define STRING      22
#define THEN        23
#define WHILE       24


//operatory inkrementace a dekrementace
#define INC         30
#define DEC         31
#define DIV         32
#define MOD         33  //mod - celociselne div
#define MULTIPLY    34
#define ASSIGNED    35


//jednotlive znaky
#define LEFT_VINCULUM   40          // '{'
#define RIGHT_VINCULUM  41          // '}'
#define SEMICOLON       42          // ';'
#define HASH            43          // '#'
#define DOTDOT          44          // '..'
#define LESS            45          // '<'
#define LESSOREQUAL     46          // '<='
#define MORE            47          // '>'
#define MOREOREQUAL     48          // '>='
#define EQUAL           49          // '=='
#define NOTEQUAL        50          // '~='
#define COLUMN          51          // ':'
#define RIGHT_BRACKET   52          // ')'
#define LEFT_BRACKET    53          // '('
#define COMMA           54          // ','
#define PARANTACIES     55          // '"'

//specialni znaky
#define END_OF_FILE    60

//chybove hlasky
#define LEX_ERROR    -1

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
int getNextToken(string *attr);
