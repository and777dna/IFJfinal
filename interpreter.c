#include "expression.h"
#include "stdbool.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "scanner.h"


int count_start = 1;
int count_end = 1;

void GEN_START_MAIN(){
	printf(".IFJcode20\n");
    printf("DEFVAR GF@_\n");

    // Global variables for concatenation processing
    printf("DEFVAR GF@_str1\n");
    printf("DEFVAR GF@_str2\n");
    printf("DEFVAR GF@_strRes\n");

    // If divide by zero
    printf("DEFVAR GF@_tmpDividingByZero\n");
    printf("JUMP enderror9\n");
    printf("LABEL error9\n");
    printf("EXIT int@9\n");
    printf("LABEL enderror9\n");


	printf("JUMP $$main\n");

 }

void GEN_WRITE_VAR_LITERAL(int token, char *attr){
    switch (token)
	{
		case INT:
			printf("int@%s\n", attr);
			break;
		case FLOAT:
			printf("float@%s\n", attr);
			break;
		case RETEZEC:
			printf("string@%s\n", attr);
			break;
		case NIL:
		    printf("nil@%s\n", attr);
            break;
        case ID:
            printf("LF@%s$%d\n", attr, 1);
			break;
	}
}

void GEN_PRINT_WRITE(int token, string attr){
    printf("WRITE ");
    GEN_WRITE_VAR_LITERAL(token, attr.str);
    printf("\n");
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

void GEN_DEFVAR_VAR(SeznamOfVars *param){
	while(param != NULL){
		printf("DEFVAR LF@%s\n", param->name);
		if(param->next != NULL){
			param = param->next;
		}
		else{
			return;
		}
    }
}

void GEN_FUNC_MAIN_END(char* name){
    printf("DEFVAR TF@%d\n",count_start);
	printf("MOVE TF@%d, %s\n",count_start, name);
	count_start++;
	return;
}

void GEN_FUNC_CALL(char *name_func, SeznamOfVars *param){
	printf("CALL $%s\n", name_func);
	while(param != NULL){
		printf("MOVE LF@%s, TF@ret%d\n",param->name, count_end);
		param = param->next;
		count_end++;
		return;
    }
}

void GEN_END_OF_FUNCTION(string attr){
    if(!strcmp(attr.str, "main")){
        printf("POPFRAME\n\n");
        printf("JUMP $END_OF_PROGRAM\n");
    } else {
        printf("POPFRAME \n");
        printf("RETURN \n\n");
    }
}

void EXPRESSION_FUNC(char *attr, int token, bool end, char* var_name){
	static char *param1;
	static char *param2;
	static int oper;
	static int counter;
	static int type;
	if (counter == 0){
		printf("Counter: %d\n", counter);
		type = token;
		param1 = attr;
		printf("  Param1: %s\n", param1);
		counter++;
	}
	else if (counter == 1){
		printf("Counter: %d\n", counter);
		param2 = attr;
		printf("  Param2: %s\n", param2);
		counter++;
	}
	else if (counter == 2){
		printf("Counter: %d\n", counter);
		oper = token;
		printf("  Operand->type: %d\n", oper);
		counter++;
	}

	if (counter == 3){
		printf("Counter-switch: %d\n", counter);
		if (oper != NULL){
			printf("Counter-switch: %d\n", counter);
			switch (oper)
			{
				case INC:
					printf("DEFVAR LF@ADD_RES\n");
					printf("ADD LF@ADD_RES LF@%s LF@%s\n", param1, param2);
					param1 = "SUM_RES"; 
					param2 = NULL;
					break;
				case DEC:
					printf("DEFVAR LF@SUB_RES\n");
					printf("DEC LF@SUB_RES LF@%s LF@%s\n", param1, param2);
					param1 = "SUB_RES"; 
					param2 = NULL;
					break;
				case MULTIPLY:
					printf("DEFVAR LF@MUL_RES\n");
					printf("MUL LF@MUL_RES LF@%s LF@%s\n", param1, param2);
					param1 = "MUL_RES"; 
					param2 = NULL;
					break;
				case DIV:
					printf("DEFVAR LF@DIV_RES\n");
					printf("DIV LF@DIV_RES LF@%s LF@%s\n", param1, param2);
					param1 = "DIV_RES"; 
					param2 = NULL;
					break;
				case MOD:
					printf("DEFVAR LF@IDIV_RES\n");
					printf("IDIV LF@IDIV_RES LF@%s LF@%s\n", param1, param2);
					param1 = "IDIV_RES"; 
					param2 = NULL;
					break;
				case HASH:
					printf("DEFVAR LF@HASH_RES\n");
					printf("STRLEN LF@HASH_RES LF@%s\n", param1);
					param1 = "HASH_RES";
					param2 = NULL;
					break;
				case DOTDOT:
					printf("DEFVAR LF@DOTDOT_RES\n");
					printf("CONCAT LF@DOTDOT LF@%s LF@%s\n", param1, param2);
					param1 = "DOTDOT_RES";
					param2 = NULL;
					break;
			}
		}
	}
	if(end && type != 38){
		printf("Counter-move: %d\n", counter);
		printf("Operator-move: %d\n", oper);
		printf(" MOVE LF@%s ", var_name);
		GEN_WRITE_VAR_LITERAL(type, param1);
		counter = 0;
	}
	else if(type == 38){
		counter = 0;
		type = 0;
	}
}

//INBUILDS FUNCTION

void GENERATION_READI(){
printf("LABEL $READI\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ret1\n");
    printf("DEFVAR LF@ret2\n");
    printf("READ LF@ret1 int\n");
    printf("DEFVAR LF@ret_check\n");

    printf("TYPE LF@ret_check LF@ret1\n");
    printf("JUMPIFNEQ $READI_END LF@ret_check string@int\n");

    printf("PUSHS LF@ret1\n");
    printf("MOVE LF@ret2 int@0\n");
    printf("PUSHS LF@ret2\n");
    printf("JUMP $END_READI\n");

	printf("LABEL $READI_END\n");
    printf("PUSHS LF@ret1\n");
    printf("MOVE LF@ret2 int@1\n");
    printf("PUSHS LF@ret2\n");

    printf("LABEL $END_READI\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");
}

void GENERATION_READS(){
    printf("LABEL $READS\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ret1\n");
    printf("DEFVAR LF@ret2\n");
    printf("READ LF@ret1 string\n");
    printf("DEFVAR LF@ret_check\n");

    printf("TYPE LF@ret_check LF@ret1\n");
    printf("JUMPIFNEQ $READS_END LF@ret_check string@string\n");

    printf("PUSHS LF@ret1\n");
    printf("MOVE LF@ret2 int@0\n");
    printf("PUSHS LF@ret2\n");
    printf("JUMP $END_READS\n");

	printf("LABEL $READS_END\n");
    printf("PUSHS LF@ret1\n");
    printf("MOVE LF@ret2 int@1\n");
    printf("PUSHS LF@ret2\n");

    printf("LABEL $END_READS\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");
}

void GENERATION_READN(){
    printf("LABEL $READN\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ret\n");
    printf("DEFVAR LF@ret2\n");
    printf("DEFVAR LF@ret_check\n");
    printf("READ LF@ret1 float\n");

    printf("TYPE LF@ret_check LF@ret1\n");
    printf("JUMPIFNEQ $INPUTF_END LF@ret_check string@float\n");

    printf("PUSHS LF@ret\n");
    printf("MOVE LF@ret2 int@0\n");
    printf("PUSHS LF@ret2\n");
    printf("JUMP $END_READN\n");

	printf("LABEL $READN_END\n");
    printf("MOVE LF@ret nil@nil\n");
    printf("PUSHS LF@ret\n");
    printf("MOVE LF@ret2 int@1\n");
    printf("PUSHS LF@ret2\n");

    printf("LABEL $END_READN\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");
}

void GENERATION_TOINTEGER(){
	printf("LABEL $TOINTEGER\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@ret1\n");
	printf("DEFVAR LF@param\n");
	printf("MOVE LF@param LF@f$0\n");//f
	printf("FLOAT2INT LF@ret1 LF@param\n");
	printf("PUSHS LF@ret1\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");
}

void GENERATION_SUBSTR(){
	printf("LABEL $SUBSTR\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@ret1\n");//string
	printf("DEFVAR LF@ret2\n");//int

	printf("DEFVAR LF@string\n");
	printf("DEFVAR LF@from\n");
	printf("DEFVAR LF@length_of_str\n");
	printf("DEFVAR LF@length\n");

	printf("DEFVAR LF@length_helper\n");
	printf("DEFVAR LF@char\n");
	printf("DEFVAR LF@new_strlen\n");

	printf("MOVE LF@ret1 string@\n");
	printf("MOVE LF@string LF@s$0\n");
	printf("MOVE LF@from LF@i$0\n");
	printf("MOVE LF@length_of_str LF@n$0\n");

	printf("MOVE LF@length_helper int@0\n");

	printf("STRLEN LF@length LF@string\n");//length = STRLEN(STRING)
	printf("SUB LF@new_strlen LF@length int@1\n");//new_strlen = length - 1

	printf("DEFVAR LF@result\n");
	printf("LT LF@result LF@length_of_str int@0\n"); //n < O
	printf("JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	printf("EQ LF@result LF@length_of_str int@0\n"); //n == O
	printf("JUMPIFEQ $SUBSTR_EMPTY LF@result bool@true\n");

	printf("LT LF@result LF@from int@0\n"); //i < O
	printf("JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	printf("GT LF@result LF@from LF@new_strlen\n");//i >= length - 1
	printf("JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	printf("ADD LF@length_helper LF@length_helper LF@from\n");
	printf("ADD LF@length_helper LF@length_helper LF@length_of_str\n");//i + n

	printf("GT LF@result LF@length_helper LF@length\n");//i + n > n ? n : i+n
	printf("JUMPIFEQ $SUBSTR_LEN LF@result bool@true\n");
	printf("JUMP $FOR_LOOP\n");
	printf("LABEL $SUBSTR_LEN\n");
	printf("MOVE LF@length_helper LF@length\n");

	printf("LABEL $FOR_LOOP\n");
	printf("JUMPIFEQ $SUBSTR_RET_0 LF@length_helper LF@from\n");
	printf("GETCHAR LF@char LF@string LF@from\n");
	printf("CONCAT LF@ret1 LF@ret1 LF@char\n");
	printf("ADD LF@from LF@from int@1\n");
	printf("JUMP $FOR_LOOP\n");

	printf("LABEL $SUBSTR_RET_0\n");
	printf("PUSHS LF@ret1\n");
	printf("MOVE LF@ret2 int@0\n");
	printf("PUSHS LF@ret2\n");
	printf("JUMP $END\n");

	printf("LABEL $SUBSTR_END\n");
	printf("MOVE LF@ret1 nil@nil\n");
	printf("PUSHS LF@ret1\n");
	printf("MOVE LF@ret2 int@1\n");
	printf("PUSHS LF@ret2\n");

	printf("LABEL $END\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");

	printf("LABEL $SUBSTR_EMPTY\n");
	printf("MOVE LF@ret1 string@\n");
	printf("PUSHS LF@ret1\n");
	printf("MOVE LF@ret2 int@0\n");
	printf("PUSHS LF@ret2\n");
	printf("JUMP $END\n\n");
}

void GENERATION_ORD(){
	printf("LABEL $ORD\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@string\n");
	printf("DEFVAR LF@int\n");
	printf("DEFVAR LF@length\n");
	printf("DEFVAR LF@right_int\n");
	printf("DEFVAR LF@ret1\n");//string
	printf("DEFVAR LF@ret2\n");//int

	printf("MOVE LF@string LF@s$0\n");
	printf("MOVE LF@int LF@i$0\n\n");

	printf("STRLEN LF@length LF@string\n");//5
	printf("SUB LF@length LF@length int@1\n");//4

	printf("GT LF@right_int LF@int LF@length\n");
	printf("JUMPIFEQ $ORD_END LF@right_int bool@true\n");//i > len(n)-1

	printf("LT LF@right_int LF@int int@0\n");
	printf("JUMPIFEQ $ORD_END LF@right_int bool@true\n");//i < 0

	printf("STRI2INT LF@ret1 LF@string LF@int\n");

	printf("PUSHS LF@ret1\n");
	printf("MOVE LF@ret2 int@0\n");
	printf("PUSHS LF@ret2\n");
	printf("JUMP $ORD_RET\n");

	printf("LABEL $ORD_END\n");
	printf("MOVE LF@ret1 nil@nil\n");
	printf("PUSHS LF@ret1\n");
	printf("MOVE LF@ret2 int@1\n");
	printf("PUSHS LF@ret2\n");

	printf("LABEL $ORD_RET\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n");
}

void GENERATION_CHR(){
printf("LABEL $CHR\n");
printf("PUSHFRAME\n");
printf("DEFVAR LF@int\n");
printf("DEFVAR LF@right_int\n");
printf("DEFVAR LF@ret1\n");//string
printf("DEFVAR LF@ret2\n");//int

printf("MOVE LF@int LF@i$0\n");

printf("LT LF@right_int LF@int int@0\n");
printf("JUMPIFEQ $CHR_END LF@right_int bool@true\n");//i < 0

printf("GT LF@right_int LF@int int@255\n");
printf("JUMPIFEQ $CHR_END LF@right_int bool@true\n");//i > len(n)-1

printf("INT2CHAR LF@ret1 LF@int\n");

printf("PUSHS LF@ret1\n");
printf("MOVE LF@ret2 int@0\n");
printf("PUSHS LF@ret2\n");
printf("JUMP $CHR_RET\n");

printf("LABEL $CHR_END\n");

printf("MOVE LF@ret1 nil@nil\n");
printf("PUSHS LF@ret1\n");
printf("MOVE LF@ret2 int@1\n");
printf("PUSHS LF@ret2\n");

printf("LABEL $CHR_RET\n");
printf("POPFRAME\n");
printf("RETURN\n\n");
}

void GEN_CALL_INBUILDS(){
    GENERATION_READS();
    GENERATION_READI();
    GENERATION_READN();
    GENERATION_TOINTEGER();
    GENERATION_SUBSTR();
    GENERATION_ORD();
    GENERATION_CHR();
}

// void FUNC_WHILE(string attr, int count, bool if_condition, bool for_condition){
//     if(for_condition){
//         printf("LABEL $%s$for$%d\n", token->data, count);
//     } else if (if_condition){
//         printf("LABEL $%s$if$%d$else\n", token->data, count);
//     } else 
//         printf("LABEL $%s$if$%d$end\n", token->data, count);
// }


// LABEL while
// JUMPIFEQ end GF@counter string@aaaaaaaaaaaaa
// WRITE GF@counter
// WRITE string@\010
// CONCAT GF@counter GF@counter string@a
// JUMP while

// LABEL end
