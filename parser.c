#include "scanner.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include "expression.h"

//------------Global variables------------------------

int token;          // globalni promenna, ve ktere bude ulozen aktualni token
string attr;        // globalni promenna, ve ktere bude ulozen atribut tokenu
char *name_func_save;
int deep;
symtable *table;
int error_flag;
bool if_spotted;
bool while_spotted;
SeznamOfVars *seznam;
SeznamOfVars *head;

//-------------

void initSeznam()
{
    seznam = malloc(sizeof(struct seznam));
    if (seznam == NULL)
    {
        printf("Return: MEM_ALLOC_ERROR\n");
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
        printf("Return: MEM_ALLOC_ERROR\n");
        return;
    }
    if (head == NULL){
        initSeznam();
    }
    tmp->name = calloc(strlen(src) + 1, sizeof(char));
    tmp->next = NULL;
    strcpy(tmp->name, src);
    if (head == NULL)
    {   
        head = malloc(sizeof (struct seznam));
        head->name = calloc(strlen(src) + 1, sizeof(char));
        strcpy(head->name, src);
        head->next = NULL;
        seznam = tmp;
        seznam->first = head;
    }
    else
    {
        seznam->next = tmp;
        seznam = seznam->next;
        seznam->first = head;
        if (head->next == NULL){
            head->next = seznam;
        }
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
    free(seznam);
    seznam = NULL;
    return;
}

int tryGetToken()
{
    if (attr.str != NULL){
        strFree(&attr);
    }
    if ((token = getNextToken(&attr)) == -10)
    {
        changeError(1);
        printf("Return: LEX_ERROR\n");
        return LEX_ERROR;
    }
    // printf("%d   %s\n", token, attr.str);  // to check the token
    return token;
}

void changeError(int n)
{
    if(n == -10){
        fprintf(stderr, "%d\n", 1);
        freeFunc(table->func_tree);
        strFree(&attr);
        free(table);
        free(table->var_tree);
        free(name_func_save);
        exit(1);
    }
    else{
        fprintf(stderr, "%d\n", n);
        freeFunc(table->func_tree);
        free(table);
        strFree(&attr);
        free(table->var_tree);
        free(name_func_save);
        exit(n);
    }
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
                    return SYNTAX_OK;
                }
            }
        }
        changeError(2);
        return SYNTAX_ERROR;
        break;  
    case RIGHT_BRACKET:
        return SYNTAX_OK;
        break;
    default:
        changeError(2);
        return SYNTAX_ERROR;
        break;
    }
}

int GlobalinputIsOK()
{
    switch (token)
    {
    case STRING:
    case NUMBER:
    case INTEGER:  
        insertInput(attr.str, table->func_tree, name_func_save, token);
        token = tryGetToken();
        if(token == COMMA)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                return GlobalinputIsOK();
            }
            else if(token == RIGHT_BRACKET)
            {
                return SYNTAX_OK;
            }
        }
        else if(token == RIGHT_BRACKET){
            return SYNTAX_OK;
        }
        changeError(5);
        return SEM_ERROR_FUNCPARAM;
        break;  
    case RIGHT_BRACKET:
        return SYNTAX_OK;
        break;
    default:
        changeError(5);
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
    else{
        changeError(5);
        return SEM_ERROR_FUNCPARAM;
    }
}

int GlobalCompare(funcs tmp)
{
    switch (token)
    {
    case ID:
    strcpy_for_var(attr.str);
        tmp->in->name = attr.str;
        if (token = tryGetToken() == COLUMN)
        {
            token = tryGetToken();
            if (token != tmp->in->type)
            {
                changeError(5);
                return SEM_ERROR_FUNCPARAM;
            }
            insertVar(&(table->var_tree), deep, seznam->name, token);
            if ((token = tryGetToken()) == COMMA)
            {
                token = tryGetToken();
                if (token == ID)
                {
                    tmp->in = tmp->in->next;
                    return GlobalCompare(tmp);
                }
                changeError(2);
                return SYNTAX_ERROR;
            }
            else if(token == RIGHT_BRACKET)
            {
                return SYNTAX_OK;
            }
        }
        break;
    case RIGHT_BRACKET:
        if (tmp->in == NULL)
        {
            return SYNTAX_OK;
        }
        changeError(2);
        return SYNTAX_ERROR;
        break;
    default:
        changeError(2);
        return SYNTAX_ERROR;
        break;
    }
}

int GlobalCompareOut(funcs tmp)
{
    switch (token)
    {
    case STRING:
    case INTEGER:
    case NUMBER:
        if (tmp->out->type != token)
        {
            changeError(5);
            return SEM_ERROR_FUNCPARAM;
        }
        if ((token = tryGetToken()) == COMMA)
        {
            token = tryGetToken();
            if (token == STRING || token == INTEGER || token == NUMBER)
            {
                tmp->out = tmp->out->next;
                return GlobalCompareOut(tmp);
            }
            changeError(2);
            return SYNTAX_ERROR;
        }
        return SYNTAX_OK;
        break;
    default:
        changeError(2);
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
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            GEN_START_OF_FUNCTION(attr);
            funcs tmp = findFunc(table->func_tree, name_func_save);
            if (tmp == NULL)
            {
                insertFunc(name_func_save, &(table->func_tree), origin);
            }
            else if (tmp != NULL && tmp->origin == 1)
            {
                tmp->origin = 2;
                if((token = tryGetToken()) != LEFT_BRACKET){
                    changeError(2);
                    return SYNTAX_ERROR; 
                }
                token = tryGetToken();
                globalCheck = GlobalCompare(tmp);
                if (globalCheck)
                {
                    changeError(5);
                    return SEM_ERROR_FUNCPARAM;
                }
                token = tryGetToken();
                if (token == COLUMN && tmp->out != NULL)
                {
                    token = tryGetToken();
                    globalCheck = GlobalCompareOut(tmp);
                    freeSeznam();
                    return globalCheck;
                }
                else if (token != COLUMN && tmp->out == NULL)
                {
                    return SYNTAX_OK;
                }
                else{
                    changeError(2);
                    return SYNTAX_ERROR;
                }
            }
            else if (tmp != NULL)
            {
                changeError(3);
                return SEM_ERROR_DEFINE;
            }
            name_func_save = strcpy_for_func(attr.str, name_func_save);
            token = tryGetToken();
            if (token == LEFT_BRACKET){
                token = tryGetToken();
                if (inputWasComplited = inputIsOK())
                {
                    changeError(5);
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
            else{
                changeError(2);
                return SYNTAX_ERROR; 
            }
        }
        else {
            changeError(7);
            return SEM_ERROR;
        }
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
                            changeError(5);
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
                                break;
                            }
                        }  
                    }
                    else{
                        changeError(2);
                        return SYNTAX_ERROR; 
                    }
                }
            }
        }
        else{
            changeError(2);
            return SYNTAX_ERROR;
        }
    }
    changeError(2);
    return SYNTAX_ERROR;
}

int functionBodyIsOK()
{
    while ((token != END) || deep != 0){
        switch (token)
        {
        case LOCAL:
            head = NULL;
            //freeSeznam();
            token = tryGetToken();
            if (token == ID)
            {
                vars varcheck = findVar(table->var_tree, deep, attr.str);
                if (varcheck != NULL && varcheck->deepOfVar == deep){
                    changeError(3);
                    return SEM_ERROR_DEFINE;
                }
                strcpy_for_var(attr.str);
                token = tryGetToken();
                if (token == COLUMN)
                {
                    token = tryGetToken();
                    if (token == INTEGER || token == STRING || token == NUMBER)
                    {
                        seznam = seznam->first;
                        while(seznam != NULL){
                            insertVar(&(table->var_tree), deep, seznam->name, token);
                            if(seznam->next != NULL){
                                seznam = seznam->next;
                            }
                            else{
                                break;
                            }
                        }
                        GEN_DEFVAR_VAR(seznam);
                        int type = token;
                        token = tryGetToken();
                        switch (token)
                        {
                            case ASSIGNED:
                                token = tryGetToken();
                                token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, type);
                                break;
                            default:
                                break;
                        }
                        freeSeznam();
                    }
                }
                else if(token == COMMA){
                    while (token != COLUMN){
                        if(token == COMMA){
                            token = tryGetToken();
                        }
                        vars varcheck = findVar(table->var_tree, deep, attr.str);
                        if (varcheck != NULL){
                            changeError(3);
                            return SEM_ERROR_DEFINE;
                        }
                        strcpy_for_var(attr.str);
                        token = tryGetToken();
                    }
                    if (token == COLUMN)
                    {
                        token = tryGetToken();
                        if (token == INTEGER || token == STRING || token == NUMBER)
                        {
                            seznam = seznam->first;
                            while(seznam != NULL){
                                insertVar(&(table->var_tree), deep, seznam->name, token);
                                if(seznam->next != NULL){
                                    seznam = seznam->next;
                                }
                                else{
                                    break;
                                }
                            }
                            int type = token;
                            seznam = seznam->first;
                            GEN_DEFVAR_VAR(seznam);
                            token = tryGetToken();
                            switch (token)
                            {
                                case ASSIGNED:
                                    token = tryGetToken();
                                    seznam = seznam->first;
                                    token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, type);
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
        case WRITE:
            token = tryGetToken();
            if(token != LEFT_BRACKET){
                changeError(2);
                return SYNTAX_ERROR;
            }
            token = tryGetToken();
            if(token == RIGHT_BRACKET){
                changeError(2);
                return SYNTAX_ERROR;
            }
            while(token != RIGHT_BRACKET){
                switch (token){
                case ID:;
                    vars vareh = findVar(table->var_tree, deep, attr.str);
                    if(vareh == NULL){
                        changeError(3);
                        return SEM_ERROR_DEFINE;
                    }
                    GEN_PRINT_WRITE(token, attr);
                    token = tryGetToken();
                    if(token == COMMA){
                        token = tryGetToken();
                        continue;
                    }
                    else if(token == RIGHT_BRACKET){
                        break;
                    }
                    else{
                        changeError(2);
                        return SYNTAX_ERROR;
                    }
                    break;
                case INT:
                case RETEZEC:
                case FLOAT:
                    GEN_PRINT_WRITE(token, attr);
                    token = tryGetToken();
                    if(token == COMMA){
                        token = tryGetToken();
                        continue;
                    }
                    else if(token == RIGHT_BRACKET){
                        break;
                    }
                    else{
                        changeError(2);
                        return SYNTAX_ERROR;
                    }
                    break;
                default:
                    changeError(2);
                    return SYNTAX_ERROR;
                }
            }
            token = tryGetToken();
            break;
        case WHILE:
            freeSeznam();
            whileSpotted(1);
            while_spotted = true;
            ifORwhileWasTheLast(2);
            token = tryGetToken();
            token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
            if (token != DO)
            {
                changeError(2);
                return SYNTAX_ERROR;
            }
            deep++;
            token = tryGetToken();
            break;
        case IF:
            freeSeznam();
            ifSpotted(1);
            ifORwhileWasTheLast(1);
            if_spotted = true; 
            token = tryGetToken();
            token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
            if (token != THEN)
            {
                changeError(2);
                return SYNTAX_ERROR;
            }
            deep++;
            token = tryGetToken();
            break;
        case ELSE:
            freeSeznam();
            if(!(if_spotted) && deep == 0){
                changeError(2);
                return SYNTAX_ERROR;
            }
            // if_spotted = false;
            token = tryGetToken();
            printf("JUMP ifend%d\n", ifSpotted(0));
            printf("LABEL else%d\n", ifSpotted(0));
            break;    
        case ID:;
            funcs maybefunc = findFunc(table->func_tree, attr.str);
            if (maybefunc != NULL)
            {
                token = tryGetToken();
                if (token != LEFT_BRACKET)
                {
                    changeError(2);
                    return SYNTAX_ERROR;
                }
                token = tryGetToken();
                while (token != RIGHT_BRACKET)
                {   
                    int typeCheck;
                    vars maybevar = findVar(table->var_tree, deep, attr.str);
                    if(maybevar != NULL){
                        typeCheck = maybevar->type;
                    }
                    else{
                        typeCheck = token + 15;
                    }
                    if (token == COMMA)
                    {
                        token = tryGetToken();
                        continue;
                    }
                    else if (typeCheck != maybefunc->in->type)
                    {
                        changeError(5);
                        return SEM_ERROR_FUNCPARAM;
                    }
                    if(maybefunc->in->next == NULL){
                        maybefunc->in = maybefunc->in->first;
                    }
                    else{
                        maybefunc->in = maybefunc->in->next;
                    }
                    token = tryGetToken();
                }
                GEN_FUNC_MAIN_END(attr.str, token);
                GEN_FUNC_CALL(maybefunc->name, seznam);
                token = tryGetToken();
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
                        return SYNTAX_ERROR;
                    }
                    else{
                        token = tryGetToken();
                        if (token == ID){
                            funcs maybefunc = findFunc(table->func_tree, attr.str);
                            if (maybefunc == NULL){
                                seznam = seznam->first;
                                token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
                                if (token == COMMA){
                                    token = tryGetToken();
                                    break;
                                }
                                freeSeznam();
                                break;
                            }
                            if (maybefunc != NULL){
                                seznam = seznam->first;
                                maybefunc->out = maybefunc->out->first;
                                while(seznam != NULL && maybefunc->out != NULL){
                                    vars varfind = findVar(table->var_tree, deep, seznam->name);
                                    if (maybefunc->out->next == NULL){
                                        vars varfind = findVar(table->var_tree, deep, seznam->name);
                                        if(varfind->type == maybefunc->out->type){
                                            if((seznam->next == NULL && maybefunc->out->next != NULL) 
                                            || (seznam->next != NULL && maybefunc->out->next == NULL)){
                                                changeError(5);
                                                return SEM_ERROR_FUNCPARAM;
                                            }
                                        }
                                        else{
                                            changeError(5);
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
                                                changeError(5);
                                                return SEM_ERROR_FUNCPARAM;
                                        }
                                    }
                                    else{
                                        changeError(5);
                                        return SEM_ERROR_FUNCPARAM;
                                    }
                                }
                            }
                            token = tryGetToken();
                            if (token != LEFT_BRACKET){
                                changeError(2);
                                return SYNTAX_ERROR;
                            }
                            else{
                                token = tryGetToken();
                                if (maybefunc != NULL)
                                {   
                                    if(maybefunc->in != NULL){
                                        maybefunc->in = maybefunc->in->first;
                                        int check;
                                        while(token != RIGHT_BRACKET){
                                            if (token == ID){
                                                vars varfind = findVar(table->var_tree, deep, attr.str);
                                                check = varfind->type;
                                            }
                                            else{
                                                check = token + 15;
                                            }
                                            if (check == maybefunc->in->type){
                                                if(maybefunc->in->next != NULL){
                                                    maybefunc->in = maybefunc->in->next;
                                                }
                                                GEN_FUNC_MAIN_END(attr.str, token);
                                                token = tryGetToken();
                                            }
                                            else if(token == COMMA){
                                                token = tryGetToken();
                                                continue;
                                            }
                                            else{
                                                changeError(5);
                                                return SEM_ERROR_FUNCPARAM;
                                            }
                                        }
                                        seznam = seznam->first;
                                        token = tryGetToken();
                                        GEN_FUNC_CALL(maybefunc->name, seznam);
                                        break;
                                    }
                                    else if(token != RIGHT_BRACKET){
                                        changeError(2);
                                        return SYNTAX_ERROR;
                                    }
                                    else{
                                        token = tryGetToken();
                                        GEN_FUNC_CALL(maybefunc->name, seznam);
                                        break;
                                    }
                                }
                                else {
                                    seznam = seznam->first;
                                    token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
                                    if (token == COMMA){
                                        token = tryGetToken();
                                        break;
                                    }
                                }
                            }
                        }
                        else{
                            seznam = seznam->first;
                            token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
                            if (token == COMMA){
                                token = tryGetToken();
                                break;
                            }
                        }
                    }
                }
                else{
                    changeError(2);
                    return SYNTAX_ERROR;
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
                        token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
                        break;
                    }
                    else {
                        return SEM_ERROR_DEFINE;
                    }
                }
                else{
                    token = express(token, &attr, table->var_tree, table->func_tree, deep, seznam, 3);
                    break;
                }
            }      
            break;
        case COMMA:
            break;
        case END:
            if(table->var_tree->deepOfVar != 0){ 
                table->var_tree = freeVarTree(table->var_tree);  
            }
            deep--;  
            if(seznam != NULL){
                freeSeznam();
            }
            if (ifSpotted(0) && (ifORwhileWasTheLast(0) == 1))
            {
                fprintf(stdout, "LABEL ifend%d\n", ifSpotted(0));
                if_spotted = false;
            }
            token = tryGetToken();
            if (whileSpotted(0) && (ifORwhileWasTheLast(0) == 2))
            {
                fprintf(stdout, "JUMP while%d\n", whileSpotted(0));
                fprintf(stdout, "LABEL whileend%d\n", whileSpotted(0));
                while_spotted = false;
            }
            if (if_spotted)
            {
                ifORwhileWasTheLast(1);
            }
            else if (while_spotted)
            {
                ifORwhileWasTheLast(2);
            }            
            break;
        default:
            changeError(2);
            return SYNTAX_ERROR;
        }
    }
    return SYNTAX_OK;
}

int syntaxCheck(){
    int result;
    while(token != END_OF_FILE){
        if (token == FUNCTION){
            result = functionIsOK();
            if (result != 0){
                changeError(result);
                return result;
            }
            result = functionBodyIsOK();
            if (result != 0){
                changeError(result);
                return result;
            }
            else if (token != END){
                changeError(2);
                return SYNTAX_ERROR;
            }
            else{
                checkSEEN(token);
                table->var_tree = freeAllVars(table->var_tree);
                deep = 0;
                initSeznam();
            }
        }
        else if(token == GLOBAL){
            result = functionIsOK();
            if (result != 0){
                changeError(result);
                return result;
            }
            else{
                continue;
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
                    changeError(2);
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
                        changeError(5);
                        return SEM_ERROR_FUNCPARAM;
                    }
                    maybefunc->in = maybefunc->in->next;
                    GEN_FUNC_MAIN_END(attr.str, token);
                    token = tryGetToken();
                }
            }
            else if (maybefunc == NULL){
                changeError(2);
                return SYNTAX_ERROR;
            }
            GEN_FUNC_CALL(maybefunc->name, NULL);
        }
        else{
            changeError(2);
            return SYNTAX_ERROR;
        }
        token = tryGetToken();
    }
    return SYNTAX_OK;
}

int program()
{
    table->func_tree = insertInbuiltFuncs(table->func_tree);
    deep = 0;
    error_flag = 0;
    int error_flag = 0;
    token = tryGetToken();
    if (token == REQUIRE){
        token = tryGetToken();
        if ((token == RETEZEC) && (!strCmpConstStr(&attr, "ifj21")))
        {
            token = tryGetToken();
            fprintf(stdout, ".IFJcode21\n");
            fprintf(stdout, "JUMP $main\n\n");
            GEN_CALL_INBUILDS();
            error_flag = syntaxCheck();
            if(error_flag != 0){
                changeError(error_flag);
            }
        }
        else changeError(2);
    }
    free(table->var_tree);
    free(name_func_save);
    return error_flag;
}