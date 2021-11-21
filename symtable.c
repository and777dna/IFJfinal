#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

symtable *ST;
struct inputFunc *start;
struct outputFunc *startOut;

symtable *initST(symtable *ST)
{

    ST = malloc(sizeof(struct Symtable));
    if(ST == NULL){
        return ST;
    }
    ST->func_tree = NULL;
    ST->var_tree = NULL;

    return ST;
}

////-------------------------------------------VARS----------------------------------------------////

bool insertVar(vars *var_tree, int deep, char *name, int type)     /// insert(&(ST->var_tree),...)
{
    if(*var_tree == NULL){
        *var_tree = malloc(sizeof(struct Var_tree));
        if(*var_tree == NULL){
            printf("Return: MEM_ALLOC_ERROR\n");
            return false;
        }
        (*var_tree)->name = name;
        (*var_tree)->deepOfVar = deep;
        (*var_tree)->L = NULL;
        (*var_tree)->R = NULL;
        (*var_tree)->next = NULL;
        (*var_tree)->type = type;
        printf("dobavil %s | %d\n", (*var_tree)->name, (*var_tree)->deepOfVar);
        return true;
    }
    else if ((*var_tree != NULL)&&(deep > (*var_tree)->deepOfVar))
    {
        vars tmp = malloc(sizeof(struct Var_tree));
        if (tmp == NULL)
        {
            return false;
        }
        tmp->deepOfVar = (*var_tree)->deepOfVar;
        tmp->name = (*var_tree)->name;
        tmp->L = (*var_tree)->L;
        tmp->R = (*var_tree)->R;
        tmp->next = NULL;
        tmp->type = (*var_tree)->type;
        (*var_tree)->name = name;
        (*var_tree)->deepOfVar = deep;
        (*var_tree)->L = NULL;
        (*var_tree)->R = NULL;
        (*var_tree)->next = tmp;
        (*var_tree)->type = type;
        printf("dobavil %s | %d\n", (*var_tree)->name, (*var_tree)->deepOfVar);
        return true;
    }
    else if (strcmp(name, (*var_tree)->name) < 0)
    {
        insertVar(&((*var_tree)->L), deep, name, type);
    }
    else if (strcmp(name, (*var_tree)->name) > 0)
    {
        insertVar(&((*var_tree)->R), deep, name, type);
    }
    else{
        printf("yze est'\n");
        return false;
    }
}

vars findVarFromTree(vars var_tree, int deep, char *name)
{
    if (var_tree == NULL)
    {
        printf("VAR_NOT FIND\n");
        return NULL;
    }
    
    if (strcmp(name, var_tree->name) == 0)
    {
        printf("nasel %s %d\n", var_tree->name, var_tree->deepOfVar);
        return var_tree;
    }
    else if (strcmp(name, var_tree->name) < 0)
    {
        findVarFromTree(var_tree->L, deep, name);
    }
    else if (strcmp(name, var_tree->name) > 0)
    {
        findVarFromTree(var_tree->R, deep, name);
    } 
}

vars findVar(vars var_tree, int deep, char *name)
{
    vars tmp = var_tree;
    if (var_tree != NULL)
    {
        printf("deepVar\\\\\\%d        deep %d \n", var_tree->deepOfVar, deep);
        if (deep >= var_tree->deepOfVar)
        {
            tmp = findVarFromTree(tmp, deep, name);
            if (tmp == NULL && deep > 0)
            {
                findVar(var_tree->next, deep-1, name);
            }
            else
            {
                return tmp;
            }
        }
        else
        {
            printf("VARno with deep %d and deepvar %d\n", deep, var_tree->deepOfVar);
            return NULL;
        }    
    }
    else
    {
        printf("VARno\n");
        return NULL;
    }    
    
}

vars freeVarTree(vars var)
{
    printf("VARFree %s\n", var->name);
    if (var == NULL)
    {
        return var;
    }
    vars tmp = var;
    var = tmp->next;
    if(tmp->L != NULL){
        freeVarTree(tmp->L);
    }
    if(tmp->R != NULL){
        freeVarTree(tmp->R);
    }
    printf("VARFree %s\n", tmp->name);
    tmp = NULL;
    free(tmp);
    return var; 
}

vars freeAllVars(vars var)
{
    while (var != NULL)
    {
        var = freeVarTree(var);
    }
    printf("AllVARFree\n");
    return var;
}
////-------------------------------------FUNCTION-----------------------------------------/////

funcs insertFunc(char *name, funcs *func, int orig)
{
    if (*func == NULL)
    {
        *func = malloc(sizeof(struct Func_tree));
        if (*func == NULL)
        {
            printf("Return: MEM_ALLOC_ERROR\n");
            return NULL;
        }
        (*func)->origin = orig;
        (*func)->name = name;
        (*func)->L = NULL;
        (*func)->R = NULL;
        (*func)->in = NULL;
        (*func)->out = NULL;
        printf("dobavil %s\n", (*func)->name);
        return (*func);
    }
    else if (strcmp((*func)->name, name) < 0)
    {
        insertFunc(name, &((*func)->L), orig);
    }
    else if (strcmp((*func)->name, name) > 0)
    {
        insertFunc(name, &((*func)->R), orig);
    }
    else
        printf("chyba\n");
    
}

funcs findFunc(funcs func_tree, char *name)              //find(sym->func_tree, name)
{
    if (func_tree != NULL)
    {
        //printf("%s func--------------functree %s\n", name, func_tree->name);
        if (strcmp(func_tree->name, name) < 0)
        {
            findFunc(func_tree->L, name);
        }
        else if (strcmp(func_tree->name, name) > 0)
        {
            findFunc(func_tree->R, name);
        }
        else if (strcmp(func_tree->name, name) == 0)
        {
            //printf("nasel func %s\n", func_tree->name);
            return func_tree;
        }
    }
    else{
        printf("FUNCno\n");  
        return NULL;
    }
    
}

void insertInput(char *name_arg, funcs func, char *name_func, int type)
{
    funcs function = findFunc(func, name_func);
    if (function == NULL)
    {
        return;
    }
    if (function->in == NULL)
    {
        function->in = malloc(sizeof(struct inputFunc));
        if (function->in == NULL)
        {
            return;
        }
        function->in->name = name_arg;
        function->in->next = NULL;
        function->in->type = type;
        start = function->in;
        function->in->first = start;
        return;
    }
    inPar new_param = function->in;
    while (new_param->next != NULL)
    {
        new_param = new_param->next;
    }
    new_param->next = malloc(sizeof(struct inputFunc));
    if (new_param->next == NULL)
    {
        return;
    }
    new_param = new_param->next;
    new_param->name = name_arg;
    new_param->next = NULL;
    new_param->type = type;
    new_param->first = start;
}

void insertOutput(funcs func, int type, char *name)
{
    funcs function = findFunc(func, name);
    if (function == NULL)
    {
        printf("Return: MEM_ALLOC_ERROR\n");
        return;
    }
    if (function->out == NULL)
    {
        function->out = malloc(sizeof(struct outputFunc));
        if (function->out == NULL)
        {
            return;
        }
        if (function->out == NULL)
        {
            return;
        }
        function->out->next = NULL;
        function->out->type = type;
        startOut = function->out;
        function->out->first = startOut;
        return;
    }
    else
    {
        outPar new_param = function->out;
        while (new_param->next != NULL)
        {
            new_param = new_param->next;
        }
        new_param->next = malloc(sizeof(struct outputFunc));
        new_param->next->first = new_param;
        new_param = new_param->next;
        new_param->next = NULL;
        new_param->type = type;
        new_param->first = startOut;
    }
    
}

void freeFunc (funcs func)
{
    if (func == NULL)
    {
        return;
    }
    freeFunc(func->L);
    freeFunc(func->R);
    inPar tmp = func->in;
    while(tmp != NULL)
    {
        inPar tmp2;
        tmp2 = tmp->next;
        free(tmp);
        tmp = tmp2;
    }  
    outPar tmp3 = func->out;
    while(tmp != NULL)
    {
        outPar tmp4;
        tmp4 = tmp3->next;
        free(tmp);
        tmp3 = tmp4;
    }   
    
    // free(func);
    func = NULL;
    return;
    
}

funcs insertInbuiltFuncs(funcs func)
{
    printf("VESTAVENE FUNKCE\n");
    insertFunc("reads", &func, 2);
    insertOutput(func, STRING, "reads");
    insertFunc("readi", &func, 2);
    insertOutput(func, INTEGER, "readi");
    insertFunc("readn", &func, 2);
    insertOutput(func, NUMBER, "readn");

    insertFunc("tointeger", &func, 2);
    insertInput("f", func, "tointeger", NUMBER);
    insertOutput(func, INTEGER, "tointeger");

    insertFunc("substr", &func, 2);
    insertInput("s", func, "substr", STRING);
    insertInput("i", func, "substr", NUMBER);
    insertInput("j", func, "substr", NUMBER);
    insertOutput(func, STRING, "substr");

    insertFunc("ord", &func, 2);
    insertInput("s", func, "ord", STRING);
    insertInput("i", func, "ord", INTEGER);
    insertOutput(func, INTEGER, "ord");

    insertFunc("chr", &func, 2);
    insertInput("i", func, "chr", INTEGER);
    insertOutput(func, STRING, "chr");
    return func;
}
////--------------------------------------------------------------------------------------------------
/*
int main()
{
    
    ST = init(ST);

    insertVar(&(ST->var_tree), 0, "abc", 1);
    insertVar(&(ST->var_tree), 0, "adsad", 1);
    insertVar(&(ST->var_tree), 0, "ggghcff", 1);
    insertVar(&(ST->var_tree), 1, "adsad", 1);
    vars tmp = findVar(ST->var_tree, 0 , "ggghcff");
    if (tmp->deepOfVar == 0)
    {
        printf("nasel\n\n");
    }
    

    Token *token = malloc(sizeof(struct token));
    token->data = "ahoj";
    token->next = NULL;
    token->size = 4;
    token->type = 10;

    insertFunc(token->data, &(ST->func_tree));
    insertInput("as", ST->func_tree, "ahoj", 2);

    freeFunc(ST->func_tree);
    freeVarTree(ST->var_tree);
    //free(ST->var_tree);
    return 0;
}*/