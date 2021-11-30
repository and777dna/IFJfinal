#include "expression.h"
#include "stdbool.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "scanner.h"

//--------------Stack-----------

Stack_t* createStack() {
    Stack_t *out;
    out = malloc(sizeof(Stack_t));
    if (out == NULL) {
        printf("Return: MEM_ALLOC_ERROR\n");
        exit(OUT_OF_MEMORY);
    }
    out->size = INIT_SIZE;
    out->attr = malloc(out->size * sizeof(T));
    if (out->attr == NULL) {
        free(out);
        printf("Return: MEM_ALLOC_ERROR\n");
        exit(OUT_OF_MEMORY);
    }
    out->top = 0;
    return out;
}
 
void deleteStack(Stack_t **stack) {
    if (*stack != NULL){
        free((*stack)->attr);
        free(*stack);
        *stack = NULL;
    }
}

void resize(Stack_t *stack) {
    stack->size *= MULTIPLIER;
    stack->attr = realloc(stack->attr, stack->size * sizeof(T));
    if (stack->attr == NULL) {
        changeError(99);
        exit(STACK_OVERFLOW);
    }
}

void push(Stack_t *stack, string value, int type) {
    if (stack->top >= stack->size) {
        resize(stack);
    }
    stack->top++;
    stack->attr[stack->top].attr = calloc(sizeof(value.str), sizeof(char));
    strcpy(stack->attr[stack->top].attr, value.str);
    stack->attr[stack->top].type = type;
    // printf("PUSH: ATTR->type: %d|| ATTR->str: %s\n", stack->attr[stack->top].type, stack->attr[stack->top].attr);
}

void pop(Stack_t *stack, int token, string attr, int deep, SeznamOfVars *seznam, bool end) { 
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);
    }
    char *name = NULL;
    if(seznam != NULL){
        name = seznam->name;
    }
    printf("POP: ATTR->type: %d|| ATTR->str: %s\n", stack->attr[stack->top].type, stack->attr[stack->top].attr);
    EXPRESSION_FUNC(stack->attr[stack->top].attr, stack->attr[stack->top].type, end, name);
    stack->top--;
}

void implode(Stack_t *stack) {
    stack->size = stack->top;
    stack->attr = realloc(stack->attr, stack->size * sizeof(T));
}

//-----------------PrecTable----------------------

char precTable[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
{
//  +    -    \    *    /    (    )    i    $    =   ~=    <   <=    >   >=   str   #   ..   nil
  {'>', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#', '#'},  // +
  {'>', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#', '#'},  // -
  {'>', '>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#', '#'},  // \    //space after '\' is important
  {'>', '>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#', '#'},  // *
  {'>', '>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#', '#'},  // /
  {'<', '<', '<', '<', '<', '<', '=', '<', '#', '<', '<', '<', '<', '<', '<', '<', '<', '#', '#'},  // (
  {'>', '>', '>', '>', '>', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '#', '#'},  // )
  {'>', '>', '>', '>', '>', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '#', '#'},  // i
  {'<', '<', '<', '<', '<', '<', '#', '<', '#', '<', '<', '<', '<', '<', '<', '<', '<', '<', '#'},  // $
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '<'},  // =
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '<'},  // ~=
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '#'},  // <
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '#'},  // <=
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '#'},  // >
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#', '#'},  // >=
  {'>', '#', '#', '#', '#', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '>', '#'},  // str
  {'<', '<', '<', '<', '<', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '<', '#', '#', '#'},  // #
  {'#', '#', '#', '#', '#', '#', '#', '#', '>', '>', '>', '>', '>', '>', '>', '<', '#', '>', '#'},  // ..
  {'#', '#', '#', '#', '#', '#', '>', '#', '>', '>', '>', '#', '#', '#', '#', '#', '#', '#', '#'},  // nil
};

int TableCheck(Stack_t *stack, int token, string *attr, vars vartree, funcs functree, int deep, SeznamOfVars *seznam, bool end, int type){
    
    static int savetype;
    int checktype;
    if ((token < 10 || token > 25) && token != COMMA){
        int stackVal = stack->attr[stack->top].type - 30;
        int inputNum = token - 30;
        if(token == RETEZEC || token == INT || token == FLOAT){
            
            if (type == STRING || type == INTEGER){
                if (token != type-15 && token != ID && type != 3){
                    changeError(4);
                }
            }
            else if(type == NUMBER){
                if (token != INT || token != FLOAT && token != ID && type != 3){
                    changeError(4);
                }
            }
        }
        if (token == ID || token == INT || token == FLOAT){
            inputNum = 7;
        }
        else if (token == RETEZEC || token == STRING){
            inputNum = 15;
        }
        else if(token == NIL){
            if(stack->attr[stack->top].type == EQUAL || stack->attr[stack->top].type == NOTEQUAL || stack->attr[stack->top].type == 38){
                inputNum = 18;
            }
            else{
                changeError(8);
            }
        }
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
        else if (stack->attr[stack->top].type == NIL){
            if(token == EQUAL || token == NOTEQUAL){
                stackVal = 18;
            }
            else{
                changeError(8);
            }
        }
//---------------PrecTable Check------------
        if((stackVal == 2 || stackVal == 4) && attr->str == 0){
            return ZERO_DIVISION_ERROR;
        }
        if (precTable[stackVal][inputNum] == '<'){
            push(stack, *attr, token);
            token = tryGetToken();
            if (!(token > 9 && token < 48)){
                checktype = token;
                if(token == ID){
                    vars tmp2 = findVar(vartree, deep, attr->str);
                    if (tmp2 == NULL){
                        changeError(3);
                    }
                    else{
                        checktype = tmp2->type;
                    }
                }
                if (savetype == NUMBER){
                    if (checktype != INT && checktype != INTEGER && checktype != FLOAT && checktype != NUMBER){
                        changeError(6);
                    }
                }
                else if (savetype == STRING){
                    if (checktype == INT || checktype == INTEGER || checktype == FLOAT || checktype == NUMBER){
                        changeError(6);
                    }
                }
                else if (savetype == INTEGER){
                    if (checktype == RETEZEC || checktype == STRING || checktype == FLOAT || checktype == NUMBER){
                        changeError(6);
                    }
                }
            }
            if(token == ID){
                funcs tmp = findFunc(functree, attr->str);
                if (tmp != NULL){
                    return token;
                }
                vars tmp2 = findVar(vartree, deep, attr->str);
                if (tmp2 == NULL){
                    changeError(6);
                    return SEM_ERROR_EXPRESS;
                }
                if(tmp2->type == STRING){
                    token = 3;
                }
                else if (tmp2->type == INTEGER || tmp2->type == NUMBER){
                    token = 0;
                }
                else if (tmp2->type == NIL){
                    if(stack->attr[stack->top].type == EQUAL || stack->attr[stack->top].type == NOTEQUAL){
                        stackVal = 18;
                    }
                    else{
                        changeError(8);
                    }
                }
            }
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end, type);
        }
        else if (precTable[stackVal][inputNum] == '>'){
            if (stack->attr[stack->top].type == INTEGER || stack->attr[stack->top].type == NUMBER || 
                stack->attr[stack->top].type == STRING || stack->attr[stack->top].type == NIL){
                savetype = stack->attr[stack->top].type;
            }
            else if(stack->attr[stack->top].type == ID){
                vars tmp2 = findVar(vartree, deep, stack->attr[stack->top].attr);
                savetype = tmp2->type;
            }
            pop(stack, token, *attr, deep, seznam, end);
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end, type);
        }
        else if (precTable[stackVal][inputNum] == '='){
            if (stack->attr[stack->top].type == INTEGER || stack->attr[stack->top].type == NUMBER || 
                stack->attr[stack->top].type == STRING || stack->attr[stack->top].type == NIL){
                savetype = stack->attr[stack->top].type;
            }
            else if(stack->attr[stack->top].type == ID){
                vars tmp2 = findVar(vartree, deep, stack->attr[stack->top].attr);
                savetype = tmp2->type;
            }
            pop(stack, token, *attr, deep, seznam, end);
            token = tryGetToken();
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end, type);
        }
        else{
            if(stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INTEGER || stack->attr[stack->top].type == INT
                || stack->attr[stack->top].type == FLOAT || stack->attr[stack->top].type == NUMBER){
                if (token == INT){
                    if(strstr(attr->str, "-") != NULL){
                        char *saveptr;
                        string foo;
                        pop(stack, token, *attr, deep, seznam, end);
                        foo.str = malloc(sizeof(attr->str));
                        foo.str = strtok_r(attr->str, "-", &saveptr);
                        if (precTable[stackVal][1] == '<'){
                            push(stack, *attr, 31);
                        }
                        else if (precTable[stackVal][1] == '>'){
                            pop(stack, token, *attr, deep, seznam, end);
                            push(stack, *attr, 31);
                        }
                        else if (precTable[stackVal][1] == '='){
                            pop(stack, token, *attr, deep, seznam, end);
                            push(stack, *attr, 31);
                        }
                        attr->str = malloc(sizeof(saveptr));
                        strcpy(attr->str, foo.str);
                        return TableCheck(stack, token, attr, vartree, functree, deep, seznam, end, type); 
                    }
                }
            }
            funcs tmp = findFunc(functree, attr->str);
            vars tmp2 = findVar(vartree, deep, attr->str);
            if (tmp != NULL || tmp2 != NULL){
                token = ID;
                pop(stack, token, *attr, deep, seznam, true);
                return token;
            }
            changeError(6);
            return SEM_ERROR_EXPRESS;
        }
    }
    else if(token == COMMA){
        return token;
    }
    else{
        end = true;
        int stackVal = stack->attr[stack->top].type - 30;
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT 
            || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
        else if (stack->attr[stack->top].type == NIL){
            stackVal = 18;
        }
        if (precTable[stackVal][8] == '>'){
            if (token == NIL){
                char *name = NULL;
                if(seznam != NULL){
                    name = seznam->name;
                }
                EXPRESSION_FUNC("nil", token, false, name);
            }
            pop(stack, token, *attr, deep, seznam, true);
            return token;
        }
        else if (precTable[stackVal][8] == '='){
            pop(stack, token, *attr, deep, seznam, true);
            return token;
        }
        else{
            changeError(6);
            return SEM_ERROR_EXPRESS;
        }
    }
}

int express(int token, string *attr, vars vartree, funcs functree, int deep, SeznamOfVars *seznam, int type)
{
    bool end = false;
    Stack_t *stack = createStack();
    string buk;
    buk.str = "$";
    push(stack, buk, 38);
    if (token == ID || token == RETEZEC || token == LEFT_BRACKET || token == INT || token == FLOAT || token == HASH || token == NIL)
    {
        funcs tmp = findFunc(functree, attr->str);
        if (tmp != NULL){
            return token;
        }
        if (token == ID){
            vars tmp2 = findVar(vartree, deep, attr->str);
            if(tmp2->type == STRING){
                token = 3;
            }
            else if (tmp2->type == INTEGER || tmp2->type == NUMBER){
                token = 0;
            }
            if(tmp2->type != token && token != ID){
                changeError(4);
            }
        }
        if (token != type-15 && token != ID && type != 3 && token != NIL){
            changeError(4);
        }
        push(stack, *attr, token);
        token = tryGetToken();
        if (token == ID){
            vars tmp2 = findVar(vartree, deep, attr->str);
            if(tmp2->type == STRING){
                token = 3;
            }
            else if (tmp2->type == INTEGER || tmp2->type == NUMBER){
                token = 0;
            }
        }
        token = TableCheck(stack, token, attr, vartree, functree, deep, seznam, end, type);
        if (token == COMMA){
            if (stack != NULL){
                end = true;
                while (strcmp(stack->attr[stack->top].attr, "$") != 0){
                    pop(stack, token, *attr, deep, seznam, end);
                }
                free((stack)->attr);
                free(stack);
                stack = NULL;
            }
            token = tryGetToken();
            if(seznam != NULL){
                seznam = seznam->next;
            }
            token = express(token, attr, vartree, functree, deep, seznam, type);
        }
        else{
            end = true;
            if (token == NIL){
                if (stack != NULL){
                    while (strcmp(stack->attr[stack->top].attr, "$") != 0){
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                token = tryGetToken();
                return token; 
            }
            else if ((token >= 10 && token <= 25) || token == ID){
                if (stack != NULL){
                    while (strcmp(stack->attr[stack->top].attr, "$") != 0){
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                return token; 
            }
            else{
                if (stack != NULL){
                    while (strcmp(stack->attr[stack->top].attr, "$") != 0){
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                changeError(6);
                return SEM_ERROR_EXPRESS;
            }
        }
    }
    else{
        changeError(6);
        return SEM_ERROR_EXPRESS;
    }
}