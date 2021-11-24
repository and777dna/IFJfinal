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
        printf("Return: MEM_ALLOC_ERROR\n");
        exit(STACK_OVERFLOW);
    }
}

void push(Stack_t *stack, string value, int type) {
    if (stack->top >= stack->size) {
        resize(stack);
    }
    stack->top++;
    stack->attr[stack->top].attr = malloc(sizeof(string));
    strcpy(stack->attr[stack->top].attr, value.str);
    stack->attr[stack->top].type = type;
    printf("PUSH: ATTR->type: %d|| ATTR->str: %s\n", stack->attr[stack->top].type, stack->attr[stack->top].attr);
}

void pop(Stack_t *stack, int token, string attr, int deep, SeznamOfVars *seznam, bool end) { 
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);
    }
    char *name = seznam->name;
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
//  +    -    \    *    /    (    )    i    $    =   ~=    <   <=    >   >=   str   #   ..   nill
  {'>', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<', '<', '#', '#'},  // +
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
  {'#', '#', '#', '#', '#', '#', '>', '#', '>', '>', '>', '#', '#', '#', '#', '#', '#', '#', '#'},  // nill
};

int TableCheck(Stack_t *stack, int token, string *attr, vars vartree, funcs functree, int deep, SeznamOfVars *seznam, bool end){
    if ((token < 10 || token > 25) && token != COMMA){
        int stackVal = stack->attr[stack->top].type - 30;
        int inputNum = token - 30;
        if (token == ID || token == INT || token == FLOAT){
            inputNum = 7;
        }
        else if (token == RETEZEC || token == STRING){
            inputNum = 15;
        }
        else if(token == NIL){
            inputNum = 18;
        }
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
        else if (stack->attr[stack->top].type == NIL){
            stackVal = 18;
        }
//---------------PrecTable Check------------
        if((stackVal == 2 || stackVal == 4) && attr->str == 0){
            return ZERO_DIVISION_ERROR;
        }
        if (precTable[stackVal][inputNum] == '<'){
            push(stack, *attr, token);
            token = tryGetToken();
            if(token == ID){
                funcs tmp = findFunc(functree, attr->str);
                if (tmp != NULL){
                    return token;
                }
                vars tmp2 = findVar(vartree, deep, attr->str);
                if (tmp2 == NULL){
                    printf("Return: SEM_ERROR_EXPRESS\n");
                    return SEM_ERROR_EXPRESS;
                }
                if(tmp2->type == STRING){
                    token = 3;
                }
                else if (tmp2->type == INTEGER || tmp2->type == NUMBER){
                    token = 0;
                }
            }
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end);
        }
        else if (precTable[stackVal][inputNum] == '>'){
            pop(stack, token, *attr, deep, seznam, end);
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end);
        }
        else if (precTable[stackVal][inputNum] == '='){
            pop(stack, token, *attr, deep, seznam, end);
            token = tryGetToken();
            TableCheck(stack, token, attr, vartree, functree, deep, seznam, end);
        }
        else{ 
            funcs tmp = findFunc(functree, attr->str);
            vars tmp2 = findVar(vartree, deep, attr->str);
            if (tmp != NULL || tmp2 != NULL){
                token = ID;
                pop(stack, token, *attr, deep, seznam, end);
                return token;
            }
            printf("%s:%d -- %d\n",attr->str, inputNum, stackVal);
            printf("3 %d\n", token);
            printf("Return: SEM_ERROR_EXPRESS\n");
            return SEM_ERROR_EXPRESS;
        }
    }
    else if(token == COMMA){
        return token;
    }
    else{
        end = true;
        int stackVal = stack->attr[stack->top].type - 30;
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
        if (precTable[stackVal][8] == '>'){
            printf("EXPRESS_POP\n");
            pop(stack, token, *attr, deep, seznam, end);
            return token;
        }
        else if (precTable[stackVal][8] == '='){
            printf("EXPRESS_POP\n");
            pop(stack, token, *attr, deep, seznam, end);
            return token;
        }
        else{ 
            printf("Return: SEM_ERROR_EXPRESS\n");
            return SEM_ERROR_EXPRESS;
        }
    }
}

int express(int token, string *attr, vars vartree, funcs functree, int deep, SeznamOfVars *seznam)
{
    printf("SEZNAM->FIRST: %s\n", seznam->name);
    bool end = false;
    Stack_t *stack = createStack();
    string buk;
    buk.str = "$";
    push(stack, buk, 38);
    if (token == ID || token == RETEZEC || token == LEFT_BRACKET || token == INT || token == FLOAT || token == HASH)
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
        token = TableCheck(stack, token, attr, vartree, functree, deep, seznam, end);
        if (token == COMMA){
            token = tryGetToken();
            if(seznam != NULL){
                seznam = seznam->next;
            }
            token = express(token, attr, vartree, functree, deep, seznam);
        }
        else{
            end = true;
            if (token == NIL){
                if (stack != NULL){
                    if (strcmp(stack->attr, "$") != 0){
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                printf("EXPRESS_END\n");
                token = tryGetToken();
                return token; 
            }
            else if ((token >= 10 && token <= 25) || token == ID){
                if (stack != NULL){
                    if (strcmp(stack->attr, "$") != 0){
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                printf("EXPRESS_END\n");
                return token; 
            }
            else{
                if (stack != NULL){
                    if (strcmp(stack->attr, "$") != 0){
                        printf("EXPRESS_END_POP: %s\n", stack->attr);
                        pop(stack, token, *attr, deep, seznam, end);
                    }
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                printf("Return: SEM_ERROR_EXPRESS1\n");
                return SEM_ERROR_EXPRESS;
            }
        }
    }
    else{
        printf("Return: SEM_ERROR_EXPRESS2\n");
        return SEM_ERROR_EXPRESS;
    }
}