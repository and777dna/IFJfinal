
/**
 * @struct Stack item represetation.
 */
typedef struct stack_item
{
	char *attr;
	int token; /// Data type used for semantic analysis.
	struct stack_item *next; /// Pointer to next stack item.
} Stack_item;

/**
 * @struct Stack representation.
 */
typedef struct 
{
	Stack_item *top; /// Pointer to stack item on top of stack.
} Stack;