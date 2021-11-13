#define SYNTAX_OK 0
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEM_ERROR_DEFINE 3
#define SEM_ERROR_INCOMP 4
#define SEM_ERROR_FUNCPARAM 5
#define SEM_ERROR_EXPRESS 6
#define SEM_ERROR 7
#define NILL_ERROR 8
#define ZERO_DIVISION_ERROR 9
#define MEM_ALLOC_ERROR 99

int program();
void changeError(int n);
int tryGetToken();