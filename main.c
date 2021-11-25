#include "scanner.h"
#include "parser.h"
#include <stdlib.h>

int main(int argc, char *argv[]){

    symtable *table;
    table = initST(table);
    FILE *f = stdin;
    // if ((f = fopen(stdin, "r")) == NULL)
    // {
    //     printf("Soubor se nepodarilo otevrit\n");
    //     return FILE_ERROR;
    // }else{
    //     printf("Soubor se podarilo otevrit\n");
    // }
    setSourceFile(f);
    string *token;
    token = malloc(sizeof(struct Str));
    int a;
    setTable(table);
    a = program();
    changeError(a);
    // if (a == 0){
    //     printf("\x1B[33m""FINISHED WITHOUT ERRORS!\nCONGRATS!!!\n");    
    // }
    // else{
    //     printf("\x1B[31m""EXIT WITH ERROR: %d\n", a);
    // }
    freeFunc(table->func_tree);
    free(token);
    free(table);
    fclose(f);

    return 0;
}

//gcc main.c parser.c symtable.c scanner.c expression.c interpreter.c -o main && ./main