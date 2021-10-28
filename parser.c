
#include "scanner.h"
#include "parser.h"
#include "sym.h"


//tSymbolTable *table;// globalni promenna uchovavajici tabulku symbolu
//tListOfInstr *list; // globalni promenna uchovavajici seznam instrukci
int token;          // globalni promenna, ve ktere bude ulozen aktualni token
string attr;        // globalni promenna, ve ktere bude ulozen atribut tokenu
char *name_func_save;
char *name_var_save;
int deep;
int counterVar = 1;
symtable *table;

void setTable(symtable *st)
{
    table = st;
}

int inputIsOK()
{
    switch (token)
    {
    case ID:
        name_var_save = attr.str;
        token = getNextToken(&attr);
        if (token == COLUMN)
        {
            token = getNextToken(&attr);
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(name_var_save, table->func_tree, name_func_save, token);
                token = getNextToken(&attr);
                if (token == COMMA)
                {
                    token = getNextToken(&attr);
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
        name_var_save = attr.str;
        token = getNextToken(&attr);
        if (token == COMMA)
        {
            token = getNextToken(&attr);
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                return inputIsOK();
            }
            //??
            token = getNextToken(&attr);
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                insertInput(name_var_save, table->func_tree, name_func_save, token);
            }
            else if(token == RIGHT_BRACKET)
            {
                printf("OKINPUT\n\n");
                return SYNTAX_OK;
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

int outputIsOK()
{
    if (token == STRING || token == INTEGER || token == NUMBER)
    {
        insertOutput(table->func_tree, token, name_func_save);
        token = getNextToken(&attr);
        if (token == COMMA)
        {
            token = getNextToken(&attr);
            return outputIsOK();
        }
        else 
        {   
            printf("gvdfvjn%s\n\n", attr.str);
            return SYNTAX_OK;
        }    
    }
    else return SEM_ERROR_FUNCPARAM;
}

int GlobalCompare(funcs tmp)
{
    //token = getNextToken(&attr);
    switch (token)
    {
    case ID:
        tmp->in->name = attr.str;
        if (token = getNextToken(&attr) == COLUMN)
        {
            token = getNextToken(&attr);
            if (token != tmp->in->type)
            {
                return SEM_ERROR_FUNCPARAM;
            }
            if ((token = getNextToken(&attr)) == COMMA)
            {
                token = getNextToken(&attr);
                if (token == ID)
                {
                    tmp->in = tmp->in->next;
                    return GlobalCompare(tmp);
                }
                return SEM_ERROR_FUNCPARAM;
            }
            else if(token == RIGHT_BRACKET)
            {
                printf("OKINPUT\n\n");
                return SYNTAX_OK;
            }
        }
        else return SEM_ERROR_DEFINE;
        break;
    case RIGHT_BRACKET:
        if (tmp->in == NULL)
        {
            return SYNTAX_OK;
        }
        return SEM_ERROR_FUNCPARAM;
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
        if ((token = getNextToken(&attr)) == COMMA)
        {
            token = getNextToken(&attr);
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
        return SEM_ERROR_FUNCPARAM;
        break;
    }
}

int functionIsOK()
{
    bool inputWasComplited;
    bool outputWasComplited;
    int u;
    int origin;
    if (token == FUNCTION){
        origin = 2;
        token = getNextToken(&attr);
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
                token = getNextToken(&attr); //проверка
                token = getNextToken(&attr);
                u = GlobalCompare(tmp);
                if (u != 0)
                {
                    return u;
                }
                token = getNextToken(&attr);
                if (token == COLUMN && tmp->out != NULL)
                {
                    token = getNextToken(&attr);
                    u = GlobalCompareOut(tmp);
                    return u;
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
            //funcs s = findFunc(table->func_tree, attr.str);
            name_func_save = attr.str;
            //printf("%s\n", s->name);
            token = getNextToken(&attr);
            if (token == LEFT_BRACKET){
                token = getNextToken(&attr);
                if (inputWasComplited = inputIsOK())
                {
                    return SEM_ERROR_FUNCPARAM;
                }
                else
                {
                    token = getNextToken(&attr);
                    switch (token)
                    {
                    case COLUMN:
                        printf("OKOK\n\n");
                        token = getNextToken(&attr);
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
        else return SYNTAX_ERROR;
    }
    else if (token == GLOBAL){
        origin = 1;
        token = getNextToken(&attr);
        if (token == ID){
            //v tabulku
            insertFunc(attr.str, &(table->func_tree), origin);
            //funcs s = findFunc(table->func_tree, attr.str);
            name_func_save = attr.str;
            //printf("%s\n", s->name);
            token = getNextToken(&attr);
            if (token == COLUMN){
                token = getNextToken(&attr);
                if (token == FUNCTION){
                    token = getNextToken(&attr);
                    if (token == LEFT_BRACKET){
                    token = getNextToken(&attr);
                    if (inputWasComplited = GlobalinputIsOK())
                    {
                        return SYNTAX_ERROR;
                    }
                    else
                    {
                        token = getNextToken(&attr);
                        switch (token)
                        {
                        case COLUMN:
                            printf("OKOK\n\n");
                            token = getNextToken(&attr);
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

bool functionBodyIsOK()
{
    while (token != END){
        switch (token)
        {
        case LOCAL:
            token = getNextToken(&attr);
            if (token == ID)
            {
                name_var_save = attr.str;
                token = getNextToken(&attr);
                if (token == COLUMN)
                {
                    token = getNextToken(&attr);
                    if (token == INTEGER || token == STRING || token == NUMBER)
                    {
                        insertVar(&(table->var_tree), deep, name_var_save, token);
                        int type = token;
                        token = getNextToken(&attr);
                        switch (token)
                        {
                        case ASSIGNED:
                            token = getNextToken(&attr);
                            // if (token == )
                            // {
                            //     /* code */
                            // }
                            
                            break;

                        default:
                            break;
                        }
                    }
                }
                else return SYNTAX_ERROR;
            }
            return SYNTAX_ERROR;
            break;
        case WHILE:
            
        break;
        case IF:

        break;    
        
        default:
            break;
        }
    }
}

int program()
{
    deep = 0;
    bool result;
    token = getNextToken(&attr);
    if (token == REQUIRE){
        token = getNextToken(&attr);
        if ((token == RETEZEC) && (!strCmpConstStr(&attr, "ifj21"))){
            token = getNextToken(&attr);
            result = functionIsOK();
            if (result != 0)
            {
                return SYNTAX_ERROR;
            }
            result = functionBodyIsOK();
             
            printf("%d\n\n", table->func_tree->origin);
        }
        else return SYNTAX_ERROR;
    }
    // pokud aktualni token je jiny nez vyse uvedene, jedna se o syntaktickou chybu
    return SYNTAX_ERROR;
}