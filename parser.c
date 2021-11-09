#include "scanner.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include "expression.h"


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

void strcpy_for_var(char *src)
{

    SeznamOfVars *tmp = malloc(sizeof (struct seznam));
    if(tmp == NULL){
        return;
    }
    tmp->name = malloc(strlen(src) + 1);
    tmp->next = NULL;
    src = strcpy(tmp->name, src);

    if (seznam == NULL)
    {
        seznam = tmp;
        head = tmp;
    }
    else
    {
        while (seznam != NULL)
        {
            seznam = seznam->next;
        }
        seznam = tmp;
    }
    return;
}

char *strcpy_for_func(char *src, char *dst)
{
    dst = malloc(strlen(src) + 1);
    src = strcpy(dst, src);
    return src;
}

void initSeznam()
{
    // seznam = malloc(sizeof(struct seznam));
    // if (seznam == NULL)
    // {
    //     return;
    // }
    seznam = NULL;
    // seznam->name = NULL;
    // seznam->next = NULL;
    // seznam->first = NULL;
    return;
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

bool inputIsOK()
{
    switch (token)
    {
    case ID:

                        
        strcpy_for_var(attr.str);printf("OKOKOOOOOOOOO\n\n");
        token = tryGetToken();
        if (token == COLUMN)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(seznam->name, table->func_tree, name_func_save, token);
                printf("ggwp\n");
                insertVar(&(table->var_tree), deep, seznam->name, token);
                
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
                    return true;
                }
            }
        }
        return false;
        break;  
    case RIGHT_BRACKET:
        return true;
        break;
    default:
        return false;
        break;
    }
}

bool GlobalinputIsOK()
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
                return true;
            }
        }
        return false;
        break;  
    case RIGHT_BRACKET:
        return true;
        break;
    default:
        return false;
        break;
    }
}

bool outputIsOK()
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
            return true;
        }    
    }
    else return false;
}

bool GlobalCompare(funcs tmp)
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
                return false;
            }
            if ((token = tryGetToken()) == COMMA)
            {
                token = tryGetToken();
                if (token == ID)
                {
                    tmp->in = tmp->in->next;
                    return GlobalCompare(tmp);
                }
                return false;
            }
            else if(token == RIGHT_BRACKET)
            {
                printf("OKINPUT\n\n");
                return true;
            }
        }
        break;
    case RIGHT_BRACKET:
        if (tmp->in == NULL)
        {
            return true;
        }
        return false;
        break;
    default:
        return false;
        break;
    }
}

bool GlobalCompareOut(funcs tmp)
{
    switch (token)
    {
    case (STRING || INTEGER || NUMBER):
        if (tmp->out->type != token)
        {
            return false;
        }
        if ((token = tryGetToken()) == COMMA)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                tmp->in = tmp->in->next;
                return GlobalCompareOut(tmp);
            }
            return false;
        }
        return true;
        break;
    default:
        return false;
        break;
    }
}

bool functionIsOK()
{
    bool inputWasComplited;
    bool outputWasComplited;
    bool u;
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
                    return SYNTAX_ERROR; 
                }
                token = tryGetToken();
                u = GlobalCompare(tmp);
                if (!u)
                {
                    return false;
                }
                token = tryGetToken();
                if (token == COLUMN && tmp->out != NULL)
                {
                    token = tryGetToken();
                    u = GlobalCompareOut(tmp);
                    return u;
                }
                else if (token != COLUMN && tmp->out == NULL)
                {
                    return true;
                }
                else return false;
            }
            else if (tmp != NULL)
            {
                return false;
            }
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            token = tryGetToken();
            if (token == LEFT_BRACKET){
                token = tryGetToken();

                        printf("OKOK\n\n");
                if (!(inputWasComplited = inputIsOK()))
                {
                    return false;
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
            else return false; 
        }
        else return false;
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
                    if (!(inputWasComplited = GlobalinputIsOK()))
                    {
                        return false;
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
                else return false;  
                }
            }
        }
        else return false;
    }
    return false;
}

bool functionBodyIsOK()
{
    while ((token != END) && deep == 0){
        switch (token)
        {
        case LOCAL:
            printf("LOCAL HELLO\n");
            token = tryGetToken();
            if (token == ID)
            {
                
                strcpy_for_var(attr.str);
                token = tryGetToken();
                if (token == COLUMN)
                {
                    token = tryGetToken();
                    if (token == INTEGER || token == STRING || token == NUMBER)
                    {
                        insertVar(&(table->var_tree), deep, seznam->name, token);
                        int type = token;
                        token = tryGetToken();
                        switch (token)
                        {
                            case ASSIGNED:
                                token = tryGetToken();
                                token = express(token, &attr, table->var_tree);
                                break;
                            default:
                                break;
                        }
                    }
                }
                else return false;
            }
            else return false;
            break;
        case WHILE:
            token = tryGetToken();
            token = express(token, &attr, table->var_tree);
            if (token != DO)
            {
                return SYNTAX_ERROR;
            }
            deep = deep + 1;
            token = tryGetToken();
            bool func = functionBodyIsOK();
            break;
        case IF:
            token = tryGetToken();
            token = express(token, &attr, table->var_tree);
            if (token != THEN)
            {
                return SYNTAX_ERROR;
            }
            deep = deep + 1;
            token = tryGetToken();
            //bool func = functionBodyIsOK();
            break; 
        case ID:


            break;
        case RETURN:
            token = tryGetToken();
            if (token == ID || token == INT || token == FLOAT || token == RETEZEC || token == LEFT_BRACKET){
                if (token == ID){
                    funcs tmp1 = findFunc(table->func_tree, attr.str);
                    vars tmp2 = findVar(table->var_tree, deep, attr.str);
                    if (tmp1 != NULL){
                        printf(">:(\n");
                        break;
                    }
                    else if(tmp2 != NULL){
                        if(table->var_tree->type == STRING){
                            token = RETEZEC;
                        }
                        else if (table->var_tree->type == INTEGER || table->var_tree->type == NUMBER){
                            token = INT;
                        }
                        token = express(token, &attr, table->var_tree);
                    }
                    else {
                        return SEM_ERROR_DEFINE;
                    }
                }
                else{
                    token = express(token, &attr, table->var_tree);
                }
            }  
            
        break;
        default:
            printf("DEFALT\n");
                return false;
        }
    }
}



int program()
{
    deep = 0;
    error_flag = 0;
    bool result;
    initSeznam();
    token = tryGetToken();
    if (token == REQUIRE){
        token = tryGetToken();
        if ((token == RETEZEC) && (!strCmpConstStr(&attr, "ifj21"))){
            token = tryGetToken();
            result = functionIsOK();
            /*if (!result)
            {
                return SYNTAX_ERROR;
            }*/
            result = functionBodyIsOK();
             
            //printf("%d\n\n", table->func_tree->origin);
        }
        else changeError(2);
    }
    while (head)
    {
        printf("%s----->", head->name);
        head = head->next;
    }
    printf("\n");
    
    // pokud aktualni token je jiny nez vyse uvedene, jedna se o syntaktickou chybu
    //printf("\n%d\n\n", error_flag);
    free(name_func_save);
    free(name_var_save);
    return error_flag;
}