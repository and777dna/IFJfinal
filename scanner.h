#include <stdio.h>

#define ID 0
#define INT 1
#define FLOAT 2
#define RETEZEC 3

//klicova slova
#define DO 10
#define ELSE 11
#define END 12
#define FUNCTION 13
#define GLOBAL 14
#define IF 15
#define INTEGER 16
#define LOCAL 17
#define NIL 18
#define NUMBER 19
#define REQUIRE 20
#define RETURN 21
#define STRING 22
#define THEN 23
#define WHILE 24

//operatory inkrementace a dekrementace
#define ASSIGNED 29 // '='
#define INC 30      // '+'
#define DEC 31      // '-'
#define DIV 32      // '/'
#define MULTIPLY 33 // '*'
#define MOD 34      //mod - celociselne div

//jednotlive znaky
#define LEFT_BRACKET 35   // '('
#define RIGHT_BRACKET 36  // ')'
#define EQUAL 39          // '=='
#define NOTEQUAL 40       // '~='
#define LESS 41           // '<'
#define LESSOREQUAL 42    // '<='
#define MORE 43           // '>'
#define MOREOREQUAL 44    // '>='
#define HASH 46           // '#'
#define DOTDOT 47         // '..'
#define LEFT_VINCULUM 48  // '{'
#define RIGHT_VINCULUM 49 // '}'
#define SEMICOLON 50      // ';'
#define COLUMN 51         // ':'
#define COMMA 52          // ','
#define PARANTACIES 53    // '"'

//specialni znaky
#define END_OF_FILE 60
#define END_OF_LINE 61

//chybove hlasky
#define LEX_ERROR 1
#define FILE_ERROR -1

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR -2
#define STR_SUCCESS 0

//FILE* source;

typedef struct Str
{
    char *str;     // misto pro dany retezec ukonceny znakem '\0'
    int length;    // skutecna delka retezce
    int allocSize; // velikost alokovane pameti
    //int num;
} string;

int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
int getNextToken(string *attr);