#include "scanner.h"
#include "parser.h"
#include <stdlib.h>

int main(int argc, char *argv[]){

    symtable *table;
    table = initST(table);
    FILE *f;
    if ((f = fopen("ifj21.tl", "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit\n");
        return FILE_ERROR;
    }else{
        printf("Soubor se podarilo otevrit\n");
    }
    setSourceFile(f);
    string *token;
    token = malloc(sizeof(struct Str));
    //strInit(token);
    int a;

    /*for(int i = 0; i < 30; i++){
        a = getNextToken(token);
        printf("%d  :  %s\n",a, token->str);
        //printf("%s\n",*token);
    }*/
    setTable(table);
    a = program();
    printf("%d\n", a);
    freeFunc(table->func_tree);
    free(token);
    free(table);
    fclose(f);

    return 0;
}

//gcc main.c parser.c symtable.c scanner.c expression.c -o main && ./main