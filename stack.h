#include "stddef.h"
#define STACK_OVERFLOW -100
#define STACK_UNDERFLOW -101
#define OUT_OF_MEMORY -102
#define INIT_SIZE 15
#define MULTIPLIER 2

typedef int T; 

typedef struct Stack_tag {
    T *data;
    T *type;
    size_t size;
    size_t top;
} Stack_t;