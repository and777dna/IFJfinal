
#include "scanner.h"
#include "parser.h"
#include "sym.h"
//#include <stdbool.h>
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

int tryGetToken()
{
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
        name_var_save = attr.str;
        token = tryGetToken();
        if (token == COLUMN)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(name_var_save, table->func_tree, name_func_save, token);
                insertVar(&(table->var_tree), deep, name_var_save, token);
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
        name_var_save = attr.str;
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
                insertInput(name_var_save, table->func_tree, name_func_save, token);
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
            //printf("gvdfvjn%s\n\n", attr.str);
            return true;
        }    
    }
    else return false;
}

bool GlobalCompare(funcs tmp)
{
    //token = tryGetToken();
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
            funcs tmp = findFunc(table->func_tree, attr.str);
            if (tmp == NULL)
            {
                insertFunc(attr.str, &(table->func_tree), origin);
            }
            else if (tmp != NULL && tmp->origin == 1)
            {
                tmp->origin = 2;
                token = tryGetToken(); //проверка
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
            //funcs s = findFunc(table->func_tree, attr.str);
            name_func_save = attr.str;
            //printf("%s\n", s->name);
            token = tryGetToken();
            if (token == LEFT_BRACKET){
                token = tryGetToken();
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
            //v tabulku
            insertFunc(attr.str, &(table->func_tree), origin);
            //funcs s = findFunc(table->func_tree, attr.str);
            name_func_save = attr.str;
            //printf("%s\n", s->name);
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
    while (token != END){
        switch (token)
        {
        case LOCAL:
            printf("LOCAL HELLO\n");
            token = tryGetToken();
            if (token == ID)
            {
                name_var_save = attr.str;
                token = tryGetToken();
                if (token == COLUMN)
                {
                    token = tryGetToken();
                    if (token == INTEGER || token == STRING || token == NUMBER)
                    {
                        insertVar(&(table->var_tree), deep, name_var_save, token);
                        int type = token;
                        token = tryGetToken();
                        switch (token)
                        {
                        case ASSIGNED:
                            token = tryGetToken();
                            token = express(token, &attr);
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
            
        break;
        case IF:

        break; 
        case RETURN:
            token = tryGetToken();
            if (token == ID || token == INT || token == FLOAT || token == RETEZEC || token == LEFT_BRACKET){
                if (token == ID){
                    funcs tmp1 = findFunc(table->func_tree, attr.str);
                    printf("BD\n");
                    vars tmp2 = findVar(table->var_tree, deep, attr.str);
                    printf("B]\n");
                    if ((tmp1) == NULL){
                        printf(">:(\n");
                        break;
                    }
                    else if((tmp2) != NULL){
                        printf("X>\n");
                        token = express(token, &attr);
                        printf("XD\n");
                    }
                    else {
                        printf("P)\n");
                        return SEM_ERROR_DEFINE;
                    }
                }
                else{
                    printf("Xb");
                    token = express(token, &attr);
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
    // pokud aktualni token je jiny nez vyse uvedene, jedna se o syntaktickou chybu
    //printf("\n%d\n\n", error_flag);
    return error_flag;
}