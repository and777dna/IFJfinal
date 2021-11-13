#include "scanner.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include "expression.h"

//------------Global variables------------------------
//tSymbolTable *table;// globalni promenna uchovavajici tabulku symbolu
//tListOfInstr *list; // globalni promenna uchovavajici seznam instrukci
int token;          // globalni promenna, ve ktere bude ulozen aktualni token
string attr;        // globalni promenna, ve ktere bude ulozen atribut tokenu
char *name_func_save;
char *name_var_save;
int deep;
int counterVar = 1;
symtable *table;
int error_flag;
SeznamOfVars *seznam;
SeznamOfVars *head;

//-------------

void initSeznam()
{
    seznam = malloc(sizeof(struct seznam));
    if (seznam == NULL)
    {
        return;
    }
    seznam->name = NULL;
    seznam->next = NULL;
    seznam->first = NULL;
    return;
}

void strcpy_for_var(char *src)
{
    SeznamOfVars *tmp = malloc(sizeof (struct seznam));
    if(tmp == NULL){
        return;
    }
    tmp->name = malloc(strlen(src) + 1);
    tmp->next = NULL;
    src = strcpy(tmp->name, src);
    if (head == NULL)
    {
        initSeznam();
        head = tmp;
        seznam = tmp;
        seznam->first = head;
    }
    else
    {
        seznam->next = tmp;
        seznam = seznam->next;
        seznam->first = head;
    }
    return;
}

char *strcpy_for_func(char *src, char *dst)
{
    dst = malloc(strlen(src) + 1);
    src = strcpy(dst, src);
    return src;
}

void freeSeznam()
{
    if (seznam != NULL)
    {
        SeznamOfVars *tmp = seznam;
        while(tmp != NULL)
        {
            SeznamOfVars *tmp2;
            tmp2 = tmp->next;
            free(tmp);
            tmp = tmp2;
        }
        seznam->name = NULL;
        seznam->next = NULL;
    } 
    head = NULL;
    return;
}

int tryGetToken()
{
    if (attr.str != NULL){
        strFree(&attr);
    }
    if ((token = getNextToken(&attr)) == -1)
    {
        changeError(1);
    }
    printf("%d   %s\n", token, attr.str);
    return token;
}

void changeError(int n)
{
    error_flag = n;
}

void setTable(symtable *st)
{
    table = st;
}

//-------------Function check-----------------

int inputIsOK()
{
    switch (token)
    {
    case ID:            
        strcpy_for_var(attr.str);
        token = tryGetToken();
        if (token == COLUMN)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(seznam->name, table->func_tree, name_func_save, token);
                printf("ggwp\n");
                insertVar(&(table->var_tree), deep, seznam->name, token);
                freeSeznam();
                token = tryGetToken();
                if (token == COMMA)
                {
                    token = tryGetToken();
                    if (token == ID)
                    {
                        return inputIsOK();
                    }
                }
                else if(token == RIGHT_BRACKET)
                {
                    printf("OKINPUT\n\n");
                    return SYNTAX_OK;
                }
            }
        }
        return SYNTAX_ERROR;
        break;  
    case RIGHT_BRACKET:
        return SYNTAX_OK;
        break;
    default:
        return SYNTAX_ERROR;
        break;
    }
}

int GlobalinputIsOK()
{
    switch (token)
    {
    case (STRING || INTEGER || NUMBER):
        strcpy_for_var(attr.str);
        token = tryGetToken();
        if (token == COMMA)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                return inputIsOK();
            }
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(seznam->name, table->func_tree, name_func_save, token);
            }
            else if(token == RIGHT_BRACKET)
            {
                printf("OKINPUT\n\n");
                return SYNTAX_OK;
            }
        }
        return SEM_ERROR_FUNCPARAM;
        break;  
    case RIGHT_BRACKET:
        return SYNTAX_OK;
        break;
    default:
        return SEM_ERROR_FUNCPARAM;
        break;
    }
}

int outputIsOK()
{
    if (token == STRING || token == INTEGER || token == NUMBER)
    {
        insertOutput(table->func_tree, token, name_func_save);
        token = tryGetToken();
        if (token == COMMA)
        {
            token = tryGetToken();
            return outputIsOK();
        }
        else 
        {   
            return SYNTAX_OK;
        }    
    }
    else return SEM_ERROR_FUNCPARAM;
}

int GlobalCompare(funcs tmp)
{
    switch (token)
    {
    case ID:
        tmp->in->name = attr.str;
        if (token = tryGetToken() == COLUMN)
        {
            token = tryGetToken();
            if (token != tmp->in->type)
            {
                return SEM_ERROR_FUNCPARAM;
            }
            if ((token = tryGetToken()) == COMMA)
            {
                token = tryGetToken();
                if (token == ID)
                {
                    tmp->in = tmp->in->next;
                    return GlobalCompare(tmp);
                }
                return SYNTAX_ERROR;
            }
            else if(token == RIGHT_BRACKET)
            {
                printf("OKINPUT\n\n");
                return SYNTAX_OK;
            }
        }
        break;
    case RIGHT_BRACKET:
        if (tmp->in == NULL)
        {
            return SYNTAX_OK;
        }
        return SYNTAX_ERROR;
        break;
    default:
        return SYNTAX_ERROR;
        break;
    }
}

int GlobalCompareOut(funcs tmp)
{
    switch (token)
    {
    case (STRING || INTEGER || NUMBER):
        if (tmp->out->type != token)
        {
            return SEM_ERROR_FUNCPARAM;
        }
        if ((token = tryGetToken()) == COMMA)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                tmp->in = tmp->in->next;
                return GlobalCompareOut(tmp);
            }
            return SYNTAX_ERROR;
        }
        return SYNTAX_OK;
        break;
    default:
        return SYNTAX_ERROR;
        break;
    }
}

int functionIsOK()
{
    int inputWasComplited;
    int outputWasComplited;
    int globalCheck;
    int origin;
    if (token == FUNCTION){
        origin = 2;
        token = tryGetToken();
        if (token == ID){
            //v tabulku
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            funcs tmp = findFunc(table->func_tree, name_func_save);
            if (tmp == NULL)
            {
                printf("dobavil\n");
                insertFunc(name_func_save, &(table->func_tree), origin);
            }
            else if (tmp != NULL && tmp->origin == 1)
            {
                tmp->origin = 2;
                if((token = tryGetToken()) != LEFT_BRACKET){
                    printf("Return: SYNTAX_ERROR\n");
                    return SYNTAX_ERROR; 
                }
                token = tryGetToken();
                globalCheck = GlobalCompare(tmp);
                if (globalCheck)
                {
                    return SEM_ERROR_FUNCPARAM;
                }
                token = tryGetToken();
                if (token == COLUMN && tmp->out != NULL)
                {
                    token = tryGetToken();
                    globalCheck = GlobalCompareOut(tmp);
                    return globalCheck;
                }
                else if (token != COLUMN && tmp->out == NULL)
                {
                    return SYNTAX_OK;
                }
                else return SYNTAX_ERROR;
            }
            else if (tmp != NULL)
            {
                return SEM_ERROR_DEFINE;
            }
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            token = tryGetToken();
            if (token == LEFT_BRACKET){
                token = tryGetToken();
                if (inputWasComplited = inputIsOK())
                {
                    return SEM_ERROR_FUNCPARAM;
                }
                else
                {
                    token = tryGetToken();
                    switch (token)
                    {
                    case COLUMN:
                        token = tryGetToken();
                        outputWasComplited = outputIsOK();
                        return outputWasComplited;
                        break;
                    default:
                        return SYNTAX_OK;
                        break;
                    }
                }
                
            }
            else return SYNTAX_ERROR; 
        }
        else return SEM_ERROR;
    }
    else if (token == GLOBAL){
        origin = 1;
        token = tryGetToken();
        if (token == ID){
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            insertFunc(name_func_save, &(table->func_tree), origin);
            token = tryGetToken();
            if (token == COLUMN){
                token = tryGetToken();
                if (token == FUNCTION){
                    token = tryGetToken();
                    if (token == LEFT_BRACKET){
                    token = tryGetToken();
                    if ((inputWasComplited = GlobalinputIsOK()) != SYNTAX_OK)    
                    {
                        return SEM_ERROR_FUNCPARAM;
                    }
                    else
                    {
                        token = tryGetToken();
                        switch (token)
                        {
                        case COLUMN:
                            printf("OKOK\n\n");
                            token = tryGetToken();
                            outputWasComplited = outputIsOK();
                            return outputWasComplited;
                            break;
                        
                        default:
                            break;
                        }
                    }  
                }
                else return SYNTAX_ERROR;  
                }
            }
        }
        else return SYNTAX_ERROR;
    }
    return SYNTAX_ERROR;
}

int functionBodyIsOK()
{
    while ((token != END) && deep == 0){
        switch (token)
        {
        case LOCAL:
            printf("LOCAL HELLO\n");
            token = tryGetToken();
            if (token == ID)
            {
                vars varcheck = findVar(table->var_tree, deep, attr.str);
                if (varcheck != NULL){
                    printf("Return: SEM_ERROR_DEFINE\n");
                    return SEM_ERROR_DEFINE;
                }
                strcpy_for_var(attr.str);
                token = tryGetToken();
                if (token == COLUMN)
                {
                    token = tryGetToken();
                    if (token == INTEGER || token == STRING || token == NUMBER)
                    {
                        insertVar(&(table->var_tree), deep, seznam->name, token);
                        freeSeznam();
                        int type = token;
                        token = tryGetToken();
                        switch (token)
                        {
                            case ASSIGNED:
                                token = tryGetToken();
                                token = express(token, &attr, table->var_tree, table->func_tree, deep);
                                break;
                            default:
                                break;
                        }
                    }
                }
                else if(token == COMMA){
                    while (token != COLUMN){
                        if(token == COMMA){
                            token = tryGetToken();
                        }
                        strcpy_for_var(attr.str);
                        token = tryGetToken();
                    }
                    if (token == COLUMN)
                    {
                        token = tryGetToken();
                        if (token == INTEGER || token == STRING || token == NUMBER)
                        {
                            insertVar(&(table->var_tree), deep, seznam->name, token);
                            freeSeznam();
                            int type = token;
                            token = tryGetToken();
                            switch (token)
                            {
                                case ASSIGNED:
                                    token = tryGetToken();
                                    token = express(token, &attr, table->var_tree, table->func_tree, deep);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
                else return SYNTAX_ERROR;
            }
            else return SYNTAX_ERROR;
            break;
        case WHILE:
            token = tryGetToken();
            token = express(token, &attr, table->var_tree, table->func_tree, deep);
            if (token != DO)
            {
                printf("Return: SYNTAX_ERROR\n");
                return SYNTAX_ERROR;
            }
            deep = deep + 1;
            token = tryGetToken();
            bool func = functionBodyIsOK();
            break;
        case IF:
            token = tryGetToken();
            token = express(token, &attr, table->var_tree, table->func_tree, deep);
            if (token != THEN)
            {
                printf("Return: SYNTAX_ERROR\n");
                return SYNTAX_ERROR;
            }
            deep = deep + 1;
            token = tryGetToken();
            break; 
        case ID:;
            funcs maybefunc = findFunc(table->func_tree, attr.str);
            if (maybefunc != NULL)
            {
                token = tryGetToken();
                if (token != LEFT_BRACKET)
                {
                    changeError(2);
                    printf("Return: SYNTAX_ERROR\n");
                    return SYNTAX_ERROR;
                }
                token = tryGetToken();
                while (token != RIGHT_BRACKET)
                {   
                    int typeCheck = token + 15;
                    if (token == COMMA)
                    {
                        token = tryGetToken();
                        continue;
                    }
                    else if (typeCheck != maybefunc->in->type)
                    {
                        printf("Return: SEM_ERROR_FUNCPARAM\n");
                        return SEM_ERROR_FUNCPARAM;
                    }
                    maybefunc->in = maybefunc->in->next;
                    token = tryGetToken();
                }
            }
            else{
                vars maybevar = findVar(table->var_tree, deep, attr.str);
                strcpy_for_var(attr.str);
                if (maybevar != NULL){
                    token = tryGetToken();
                    if (token == COMMA){
                        token = tryGetToken();
                        continue;
                    }
                    else if (token != ASSIGNED){
                        changeError(2);
                        printf("Return: SYNTAX_ERROR\n");
                        return SYNTAX_ERROR;
                    }
                    else{
                        token = tryGetToken();
                        if (token == ID){
                            funcs maybefunc = findFunc(table->func_tree, attr.str);
                            if (maybefunc != NULL){
                                seznam = seznam->first;
                                maybefunc->out = maybefunc->out->first;
                                while(seznam != NULL && maybefunc->out != NULL){
                                    vars varfind = findVar(table->var_tree, deep, seznam->name);
                                    printf("SEZNAM: %s|%d == OUT: %d\n",seznam->name, varfind->type, maybefunc->out->type);
                                    if (maybefunc->out->next == NULL){
                                        vars varfind = findVar(table->var_tree, deep, seznam->name);
                                        printf("SEZNAM: %s|%d == OUT: %d\n",seznam->name, varfind->type, maybefunc->out->type);
                                        if(varfind->type == maybefunc->out->type){
                                            if((seznam->next == NULL && maybefunc->out->next != NULL) 
                                            || (seznam->next != NULL && maybefunc->out->next == NULL)){
                                                    printf("Return: SEM_ERROR_FUNCPARAM\n");
                                                    return SEM_ERROR_FUNCPARAM;
                                            }
                                        }
                                        else{
                                            printf("Return: SEM_ERROR_FUNCPARAM\n");
                                            return SEM_ERROR_FUNCPARAM;
                                        }
                                        break;
                                    }
                                    
                                    if(varfind->type == maybefunc->out->type){
                                        if(seznam->next != NULL){
                                            seznam = seznam->next;
                                        }
                                        maybefunc->out = maybefunc->out->next;
                                        if((seznam->next == NULL && maybefunc->out->next != NULL) 
                                        || (seznam->next != NULL && maybefunc->out->next == NULL)){
                                                printf("Return: SEM_ERROR_FUNCPARAM\n");
                                                return SEM_ERROR_FUNCPARAM;
                                        }
                                    }
                                    else{
                                        printf("Return: SEM_ERROR_FUNCPARAM\n");
                                        return SEM_ERROR_FUNCPARAM;
                                    }
                                }
                            }
                            token = tryGetToken();
                            if (token != LEFT_BRACKET){
                                printf("Return: SYNTAX_ERROR\n");
                                return SYNTAX_ERROR;
                            }
                            else{
                                token = tryGetToken();
                                if (maybefunc != NULL)
                                {
                                    maybefunc->in = maybefunc->in->first;
                                    while(token != RIGHT_BRACKET){
                                        if (token+15 == maybefunc->in->type){
                                            if(maybefunc->in->next != NULL){
                                                maybefunc->in = maybefunc->in->next;
                                            }
                                            token = tryGetToken();
                                        }
                                        else if(token == COMMA){
                                            token = tryGetToken();
                                            continue;
                                        }
                                        else{
                                            printf("Return: SEM_ERROR_FUNCPARAM\n");
                                            return SEM_ERROR_FUNCPARAM;
                                        }
                                    }
                                    token = tryGetToken();
                                    break;
                                }
                                else {
                                    token = express(token, &attr, table->var_tree, table->func_tree, deep);
                                    if (token == COMMA){
                                        token = tryGetToken();
                                        break;
                                    }
                                }
                            }
                        }
                        else{
                            token = express(token, &attr, table->var_tree, table->func_tree, deep);
                            if (token == COMMA){
                                token = tryGetToken();
                                break;
                            }
                        }
                    }
                }
            }
            break;
        case RETURN:
            token = tryGetToken();
            if (token == ID || token == INT || token == FLOAT || token == RETEZEC || token == LEFT_BRACKET){
                if (token == ID){
                    funcs tmp1 = findFunc(table->func_tree, attr.str);
                    vars tmp2 = findVar(table->var_tree, deep, attr.str);
                    if (tmp1 != NULL){
                        break;
                    }
                    else if(tmp2 != NULL){
                        if(table->var_tree->type == STRING){
                            token = RETEZEC;
                        }
                        else if (table->var_tree->type == INTEGER || table->var_tree->type == NUMBER){
                            token = INT;
                        }
                        token = express(token, &attr, table->var_tree, table->func_tree, deep);
                        break;
                    }
                    else {
                        return SEM_ERROR_DEFINE;
                    }
                }
                else{
                    token = express(token, &attr, table->var_tree, table->func_tree, deep);
                    break;
                }
            }      
            break;
        case COMMA:
            break;
        case END:
            return SYNTAX_OK;
            break;
        default:
            printf("DEFALT with %d\n", token);
            return SYNTAX_ERROR;
        }
    }
    return SYNTAX_OK;
}

int syntaxCheck(){
    while(token != END_OF_FILE){  
        int result;
        if (token == FUNCTION){
            result = functionIsOK();
            if (result != 0){
                printf("Return: CHECK_ERROR\n");
                return result;
            }
            result = functionBodyIsOK();
            if (result != 0){
                printf("Return: CHECK_ERROR\n");
                return result;
            }
            else if (token != END){
                printf("Return: SYNTAX_ERROR\n");
                return SYNTAX_ERROR;
            }
            else{
                freeVarTree(table->var_tree);
                deep = 0;
            }
        }
        else if(token == ID){
            funcs maybefunc = findFunc(table->func_tree, attr.str);
            if (maybefunc != NULL)
            {
                token = tryGetToken();
                if (token != LEFT_BRACKET)
                {
                    changeError(2);
                    printf("Return: SYNTAX_ERROR\n");
                    return SYNTAX_ERROR;
                }
                token = tryGetToken();
                while (token != RIGHT_BRACKET)
                {
                    int typeCheck = token + 15;
                    if (token == COMMA)
                    {
                        token = tryGetToken();
                        continue;
                    }
                    else if (typeCheck != maybefunc->in->type)
                    {
                        printf("Return: SEM_ERROR_FUNCPARAM\n");
                        return SEM_ERROR_FUNCPARAM;
                    }
                    maybefunc->in = maybefunc->in->next;
                    token = tryGetToken();
                }
            }
            else if (maybefunc == NULL){
                printf("Return: SYNTAX_ERROR\n");
                return SYNTAX_ERROR;
            }
        }
        else{
            printf("Return: SYNTAX_ERROR\n");
            return SYNTAX_ERROR;
        }
        token = tryGetToken();
    }
    return SYNTAX_OK;
}

int program()
{
    deep = 0;
    error_flag = 0;
    int error_flag = 0;
    token = tryGetToken();
    if (token == REQUIRE){
        token = tryGetToken();
        if ((token == RETEZEC) && (!strCmpConstStr(&attr, "ifj21")))
        {
            token = tryGetToken();
            error_flag = syntaxCheck();
            changeError(error_flag);
        }
        else changeError(2);
    }
    free(name_func_save);
    free(name_var_save);
    return error_flag;
}