#include <stdbool.h>

typedef struct inputFunc
{
	char *name;
	int type;
	struct inputFunc *next;	
}*inPar;

typedef struct outputFunc
{
	int type;
	struct outputFunc *next;	
}*outPar;

typedef struct Func_tree
{
    int origin;
    char *name;
    int len;
    struct Func_tree *L;
    struct Func_tree *R;
    outPar out;
	inPar in;
} *funcs;


typedef struct Var_tree
{
    int deepOfVar;
    char *name;
    int type;     // 16 : int, 22 : str, 19 : number
    struct Var_tree *L;
    struct Var_tree *R;
    struct Var_tree *next;
    
} *vars;

typedef struct Symtable
{
    vars var_tree;
    funcs func_tree;
}symtable;


typedef struct token {
    int type;
    char *data;
    int size;
    struct token *next;
} Token;

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
void freeFunc (funcs func);
void insertInbuiltFuncs(funcs func);