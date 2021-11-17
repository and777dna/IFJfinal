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
            printf("net pameti\n");
            return false;
        }
        (*var_tree)->name = name;
        (*var_tree)->deepOfVar = deep;
        (*var_tree)->L = NULL;
        (*var_tree)->R = NULL;
        (*var_tree)->next = NULL;
        (*var_tree)->type = type;
        printf("%s var--------------%d !DEEP\n", (*var_tree)->name, (*var_tree)->deepOfVar);
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
        if (deep <= var_tree->deepOfVar)
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
    return var;
}
////-------------------------------------FUNCTION-----------------------------------------/////

void insertFunc(char *name, funcs *func, int orig)
{
    if (*func == NULL)
    {
        *func = malloc(sizeof(struct Func_tree));
        if (*func == NULL)
        {
            return;
        }
        (*func)->origin = orig;
        (*func)->name = name;
        (*func)->L = NULL;
        (*func)->R = NULL;
        (*func)->in = NULL;
        (*func)->out = NULL;
        printf("dobavil %s\n", (*func)->name);
        return;
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
        printf("%s func--------------functree %s\n", name, func_tree->name);
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
            printf("nasel func\n");
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
        if (start == NULL){
            start = function->in;
        }
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
        if (startOut == NULL){
            startOut = function->out;
        }
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

////--------------------------------------------------------------------------------------------------
/*
void insertInbuiltFuncs(funcs func)
{
    insertFunc("reads", &func);
    insertOutput(func, 2, "reads");
    insertFunc("readi", &func);
    insertOutput(func, 1, "readi");
    insertFunc("readn", &func);
    insertOutput(func, 3, "reads");

    insertFunc("tointeger", &func);
    insertInput("f", func, "tointeger", 3);
    insertOutput(func, 1, "tointeger");

    insertFunc("substr", &func);
    insertInput("s", func, "substr", 2);
    insertInput("i", func, "substr", 3);
    insertInput("j", func, "substr", 3);
    insertOutput(func, 2, "substr");
}
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