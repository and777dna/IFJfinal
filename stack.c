#include "expression.h"
#include "stdbool.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "scanner.h"

typedef int T; 

typedef struct Stack_tag {
    T *data;
    T *type;
    size_t size;
    size_t top;
} Stack_t;

Stack_t* createStack() {
    Stack_t *out = NULL;
    out = malloc(sizeof(Stack_t));
    if (out == NULL) {
        exit(OUT_OF_MEMORY);
    }
    out->size = INIT_SIZE;
    out->data = malloc(out->size * sizeof(T));
    if (out->data == NULL) {
        free(out);
        exit(OUT_OF_MEMORY);
    }
    out->top = 0;
    return out;
}
 
void deleteStack(Stack_t **stack) {
    free((*stack)->data);
    free(*stack);
    *stack = NULL;
}

void resize(Stack_t *stack) {
    stack->size *= MULTIPLIER;
    stack->data = realloc(stack->data, stack->size * sizeof(T));
    if (stack->data == NULL) {
        exit(STACK_OVERFLOW);
    }
}

void push(Stack_t *stack, T value, T type) {
    if (stack->top >= stack->size) {
        resize(stack);
    }
    stack->data[stack->top] = value;
    stack->type[stack->top] = type;
    stack->top++;
}

T pop(Stack_t *stack) {
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);
    }
    stack->top--;
    return stack->data[stack->top];
}

T peek(const Stack_t *stack) {
    if (stack->top <= 0) {
        exit(STACK_UNDERFLOW);
    }
    return stack->data[stack->top - 1];
}

void implode(Stack_t *stack) {
    stack->size = stack->top;
    stack->data = realloc(stack->data, stack->size * sizeof(T));
}