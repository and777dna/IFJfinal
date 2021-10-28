#include <stdio.h>

#define ID          0
#define INT         1
#define FLOAT       2
#define RETEZEC     3

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
#define END_OF_LINE    61

//chybove hlasky
#define LEX_ERROR      1
#define FILE_ERROR    -1

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR   -2
#define STR_SUCCESS 0

//FILE* source;

typedef struct Str
{
    char* str;		// misto pro dany retezec ukonceny znakem '\0'
    int length;		// skutecna delka retezce
    int allocSize;	// velikost alokovane pameti
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