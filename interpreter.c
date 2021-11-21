#include "expression.h"
#include "stdbool.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "scanner.h"


int count = 1;


void GEN_START_MAIN(){
	printf(".IFJcode20\n");
    printf("DEFVAR GF@_\n");

    // Global variables for concatenation processing
    printf("DEFVAR GF@str1\n");
    printf("DEFVAR GF@str2\n");
    printf("DEFVAR GF@strRes\n");

    // If divide by zero
    printf("DEFVAR GF@tmpDividingByZero\n");
    printf("JUMP enderror9\n");
    printf("LABEL error9\n");
    printf("EXIT int@9\n");
    printf("LABEL enderror9\n");


	printf("JUMP $$main\n");

 }

void GEN_WRITE_VAR_LITERAL(int token, string attr, int deep){
    switch (token)
	{
		case INT:
			printf("int@%s\n", attr.str);
			break;

		case FLOAT:
			printf("float@%s\n", attr.str);
			break;

		case RETEZEC:
			printf("string@%s\n", attr.str);
			break;
		case NIL:
		    printf("nil@%s\n", attr.str);
	}
}
void GEN_START_OF_FUNCTION(string attr){
	if(!strcmp(attr.str, "main\n")){
		printf("LABEL $%s\n", attr.str);
		printf("PUSHFRAME \n\n");
	}
	else {
		printf("LABEL $$main\n");
		printf("CREATEFRAME\n");
		printf("PUSHFRAME\n\n");
	}
}

void GEN_FUNC_MAIN_END(char *name_func, SeznamOfVars *param){
	printf("CALL $%s\n", name_func);
	while(param != NULL){
		printf("MOVE LF@%s, TF@ret1\n",param->name);
		param = param->next;
		return;
	}
}
void GEN_FUNC_MAIN_START(char* name){
    printf("DEFVAR LF@%d\n",count);
	printf("MOVE LF@%d, %s\n",count, name);
	count++;
	return;
}
