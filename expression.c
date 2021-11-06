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
        printf("Mem error");
        exit(OUT_OF_MEMORY);
    }
    out->size = INIT_SIZE;
    out->attr = malloc(out->size * sizeof(T));
    if (out->attr == NULL) {
        free(out);
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
        exit(STACK_OVERFLOW);
    }
}

void push(Stack_t *stack, string value, int type) {
    if (stack->top >= stack->size) {
        resize(stack);
    }
    stack->top++;
    stack->attr[stack->top].attr = value.str;
    stack->attr[stack->top].type = type;
}

void pop(Stack_t *stack) {
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);
    }
    stack->top--;
}

void implode(Stack_t *stack) {
    stack->size = stack->top;
    stack->attr = realloc(stack->attr, stack->size * sizeof(T));
}
//----------------------------------------------

char precTable[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
{
//  +    -    \    *    /    (    )    i    $    =   <>    <   <=    >   >=   str   #   ..
  {'>', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<', '<', '#'},  // +
  {'>', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#'},  // -
  {'>', '>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#'},  // \    // The space after '\' is important
  {'>', '>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#'},  // *
  {'>', '>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '#', '<', '#'},  // /
  {'<', '<', '<', '<', '<', '<', '=', '<', '#', '<', '<', '<', '<', '<', '<', '<', '<', '#'},  // (
  {'>', '>', '>', '>', '>', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '#'},  // )
  {'>', '>', '>', '>', '>', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '#'},  // i
  {'<', '<', '<', '<', '<', '<', '#', '<', '#', '<', '<', '<', '<', '<', '<', '<', '<', '<'},  // $
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // =
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // <>
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // <
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // <=
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // >
  {'<', '<', '<', '<', '<', '<', '>', '<', '>', '#', '#', '#', '#', '#', '#', '<', '<', '#'},  // >=
  {'>', '#', '#', '#', '#', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '#', '>'},  // str
  {'<', '<', '<', '<', '<', '#', '>', '#', '>', '>', '>', '>', '>', '>', '>', '<', '#', '#'},  // #
  {'#', '#', '#', '#', '#', '#', '#', '#', '>', '>', '>', '>', '>', '>', '>', '<', '#', '>'},  // ..
};

int TableCheck(Stack_t *stack, int token, string *attr, vars vartree){
    if ((token < 10 || token > 24) && token != COMMA){
        int stackVal = stack->attr[stack->top].type - 30;
        int inputNum = token - 30;
        if (token == ID || token == INT || token == FLOAT){
            inputNum = 7;
        }
        else if (token == RETEZEC || token == STRING){
            inputNum = 15;
        }
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
//---------------PrecTable Check------------
        if (precTable[stackVal][inputNum] == '<'){
            push(stack, *attr, token);
            token = tryGetToken();
            if(token == ID){
                vars tmp2 = findVar(vartree, 0, attr->str);
                if (tmp2 == NULL)
                {
                    return SEM_ERROR_EXPRESS;
                }
                if(tmp2->type == STRING){
                    token = 3;
                }
                else if (tmp2->type == INTEGER || tmp2->type == NUMBER){
                    token = 0;
                }
            }
            TableCheck(stack, token, attr, vartree);
        }
        else if (precTable[stackVal][inputNum] == '>'){
            pop(stack);
            TableCheck(stack, token, attr, vartree);
        }
        else if (precTable[stackVal][inputNum] == '='){
            pop(stack);
            TableCheck(stack, token, attr, vartree);
        }
        else{ 
            return SEM_ERROR_EXPRESS;
        }
    }
    else{
        int stackVal = stack->attr[stack->top].type - 30;
        if (stack->attr[stack->top].type == ID || stack->attr[stack->top].type == INT || stack->attr[stack->top].type == FLOAT){
            stackVal = 7;
        }
        else if (stack->attr[stack->top].type == RETEZEC){
            stackVal = 15;
        }
        if (precTable[stackVal][8] == '>'){
            pop(stack);
            return token;
        }
        else if (precTable[stackVal][8] == '='){
            pop(stack);
            return token;
        }
        else{
            return SEM_ERROR_EXPRESS;
        }
    }
}

int express(int token, string *attr, vars vartree)
{
    Stack_t *stack = createStack();
    string buk;
    buk.str = "$";
    push(stack, buk, 38);
    if (token == ID || token == RETEZEC || token == LEFT_BRACKET || token == INT || token == FLOAT)
    {
        push(stack, *attr, token);
        token = tryGetToken();
        token = TableCheck(stack, token, attr, vartree);
        if (token == COMMA){
            token = tryGetToken();
            token = express(token, attr, vartree);
        }
        else{
            if (token > 10 && token < 24){
                if (stack != NULL){
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                return token; 
            }
            else{
                if (stack != NULL){
                    free((stack)->attr);
                    free(stack);
                    stack = NULL;
                }
                return SEM_ERROR_EXPRESS;
            }
        }
    }
    else return SEM_ERROR_EXPRESS;
}