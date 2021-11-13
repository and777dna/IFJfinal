#include <stdio.h>
#include <stdbool.h>

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
#define NUMBER 17
#define STRING 18
#define LOCAL 19
#define NIL 20
#define REQUIRE 21
#define RETURN 22
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
    struct Str *next;
} string;

typedef struct inputFunc
{
    char *name;
    int type;
    struct inputFunc *next;
} * inPar;

typedef struct outputFunc
{
    int type;
    struct outputFunc *next;
} * outPar;

typedef struct Func_tree
{
    int origin;
    char *name;
    int len;
    struct Func_tree *L;
    struct Func_tree *R;
    outPar out;
    inPar in;
} * funcs;

typedef struct Var_tree
{
    int deepOfVar;
    char *name;
    int type; // 16 : int, 22 : str, 19 : number
    struct Var_tree *L;
    struct Var_tree *R;
    struct Var_tree *next;

} * vars;

typedef struct Symtable
{
    vars var_tree;
    funcs func_tree;
} symtable;

typedef struct token
{
    int type;
    char *data;
    int size;
} Token;

typedef struct seznam
{
    char *name;
    struct seznam *next;
    struct seznam *first;
} SeznamOfVars;

void setTable(symtable *st);

symtable *initST(symtable *ST);
bool insertVar(vars *var_tree, int deep, char *name, int type);
vars findVarFromTree(vars var_tree, int deep, char *name);
vars findVar(vars var_tree, int deep, char *name);
void freeVarTree(vars var);
void freeAllVars(vars var);
void insertFunc(char *name, funcs *func, int orig);
funcs findFunc(funcs func_tree, char *name);
void insertInput(char *name_arg, funcs func, char *name_func, int type);
void insertOutput(funcs func, int type, char *name);
void freeFunc(funcs func);
void insertInbuiltFuncs(funcs func);

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
int express(int token, string *attr, vars var, funcs funcs, int deep);