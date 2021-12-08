#include "scanner.h"
#include "parser.h"
#include <stdlib.h>

int main(int argc, char *argv[]){
    symtable *table;
    table = initST(table);
    FILE *f = stdin;
    setSourceFile(f);
    string *token;
    token = malloc(sizeof(struct Str));
    int a;
    setTable(table);
    a = program();
    if (a != 0){
        changeError(a);    
    }
    free(token);
    free(table);
    fclose(f);

    return 0;
}