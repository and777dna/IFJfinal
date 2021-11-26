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
bool main_flag = false;

void GEN_START_MAIN(){
	fprintf(stdout, ".IFJcode20\n");
    fprintf(stdout, "DEFVAR GF@_\n");

    // Global variables for concatenation processing
    fprintf(stdout, "DEFVAR GF@_str1\n");
    fprintf(stdout, "DEFVAR GF@_str2\n");
    fprintf(stdout, "DEFVAR GF@_strRes\n");

    // If divide by zero
    fprintf(stdout, "DEFVAR GF@_tmpDividingByZero\n");
    fprintf(stdout, "JUMP enderror9\n");
    fprintf(stdout, "LABEL error9\n");
    fprintf(stdout, "EXIT int@9\n");
    fprintf(stdout, "LABEL enderror9\n");


	fprintf(stdout, "JUMP $$main\n");

 }

void GEN_WRITE_VAR_LITERAL(int token, char *attr){
    switch (token)
	{
		case INT:
			fprintf(stdout, "int@%s\n", attr);
			break;
		case FLOAT:
			fprintf(stdout, "float@%s\n", attr);
			break;
		case RETEZEC:
			fprintf(stdout, "string@%s\n", attr);
			break;
		case NIL:
		    fprintf(stdout, "nil@%s\n", attr);
            break;
        case ID:
            fprintf(stdout, "LF@%s$%d\n", attr, 1);
			break;
	}
}

void GEN_PRINT_WRITE(int token, string attr){
    fprintf(stdout, "WRITE ");
    GEN_WRITE_VAR_LITERAL(token, attr.str);
}

void GEN_START_OF_FUNCTION(string attr){
	if(strcmp(attr.str, "main")){
		fprintf(stdout, "LABEL $%s\n", attr.str);
		fprintf(stdout, "PUSHFRAME \n");
	}
	else {
		fprintf(stdout, "LABEL $main\n");
		fprintf(stdout, "CREATEFRAME\n");
		fprintf(stdout, "PUSHFRAME\n");
		main_flag = true;
	}
}

void GEN_DEFVAR_VAR(SeznamOfVars *param){
	while(param != NULL){
		fprintf(stdout, "DEFVAR LF@%s\n", param->name);
		if(param->next != NULL){
			param = param->next;
		}
		else{
			return;
		}
    }
}

void GEN_FUNC_MAIN_END(char* name){
    fprintf(stdout, "DEFVAR TF@%d\n",count_start);
	fprintf(stdout, "MOVE TF@%d, %s\n",count_start, name);
	count_start++;
	return;
}

void GEN_FUNC_CALL(char *name_func, SeznamOfVars *param){
	fprintf(stdout, "CALL $%s\n", name_func);
	while(param != NULL){
		fprintf(stdout, "MOVE LF@%s, TF@ret%d\n",param->name, count_end);
		if(param->next != NULL){
			param = param->next;
		}
		else{
			break;
		}
		count_end++;
    }
	return;
}

void GEN_END_OF_FUNCTION(string attr){
    if(!strcmp(attr.str, "main")){
        fprintf(stdout, "POPFRAME\n\n");
        fprintf(stdout, "JUMP $END_OF_PROGRAM\n");
    } else {
        fprintf(stdout, "POPFRAME\n");
        fprintf(stdout, "RETURN \n\n");
    }
}

bool ifSpotted(int spotted){
	static bool here = false;
	if(spotted == 1){
		here = true;
		return here;
	}
	else if(spotted == 0){
		here = false;
		return here;
	}
	else{
		return here;
	}
}

bool whileSpotted(int spotted){
	static bool here = false;
	if(spotted == 1){
		here = true;
		return here;
	}
	else if(spotted == 0){
		here = false;
		return here;
	}
	else{
		return here;
	}
}

bool checkSEEN(int token){
	static bool seen[13];
	static bool count = false;
	switch (token)
	{
		case END:
			for (int i = 0; i < 14; i++){
				seen[i] = false;
			}
			count = true;
			if(!main_flag){
				fprintf(stdout, "POPFRAME\n");
				fprintf(stdout, "RETURN\n\n");
			}
			return false;
			break;
		case INC:
			if(seen[0] == false){
				seen[0] = true;
				return false;
			}
			count = false;
			return seen[0];
			break;
		case DEC:
			if(seen[1] == false){
				seen[1] = true;
				return false;
			}
			count = false;
			return seen[1];
			break;
		case MULTIPLY:
			if(seen[2] == false){
				seen[2] = true;
				return false;
			}
			count = false;
			return seen[2];
			break;
		case DIV:
			if(seen[3] == false){
				seen[3] = true;
				return false;
			}
			count = false;
			return seen[3];
			break;
		case MOD:
			if(seen[4] == false){
				seen[4] = true;
				return false;
			}
			count = false;
			return seen[4];
			break;
		case HASH:
			if(seen[5] == false){
				seen[5] = true;
				return false;
			}
			count = false;
			return seen[5];
			break;
		case DOTDOT:
			if(seen[6] == false){
				seen[6] = true;
				return false;
			}
			count = false;
			return seen[6];
			break;
		case LESS:
			if(seen[7] == false){
				seen[7] = true;
				return false;
			}
			count = false;
			return seen[7];
			break;
		case MORE:
			if(seen[8] == false){
				seen[8] = true;
				return false;
			}
			count = false;
			return seen[8];
			break;
		case EQUAL:
			if(seen[9] == false){
				seen[9] = true;
				return false;
			}
			count = false;
			return seen[9];
			break;
		case LESSOREQUAL:
			if(seen[10] == false){
				seen[10] = true;
				return false;
			}
			count = false;
			return seen[10];
			break;
		case MOREOREQUAL:
			if(seen[11] == false){
				seen[11] = true;
				return false;
			}
			count = false;
			return seen[11];
			break;
		case NOTEQUAL:
			if(seen[12] == false){
				seen[12] = true;
				return false;
			}
			count = false;
			return seen[12];
			break;
		default:
			return count;
			break;
	}
}

void EXPRESSION_FUNC(char *attr, int token, bool end, char* var_name){
	static char *param1;
	static char *param2;
	static int oper = 0;
	static int counter;
	static int type;
	static int counter1 = 1;
	static bool tmp = false;
	if (counter == 0){
		type = token;
		param1 = attr;
		counter++;
		// fprintf(stdout, "Param1: %s\n", param1);
	}
	else if (counter == 1){
		if(token != 35 && token != 36){
			param2 = attr;
			counter++;
		}
		// fprintf(stdout, "Param2: %s\n", param2);
	}
	else if (counter == 2){
		oper = token;
		// fprintf(stdout, "Oper: %d\n", oper);
	}
	if (oper != 0){
		if (oper != 0){
			switch (oper)
			{
				case INC:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@SUM_RES\n");
						fprintf(stdout, "ADD LF@SUM_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@SUM_RES int@0\n");
						fprintf(stdout, "ADD LF@SUM_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "SUM_RES"; 
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case DEC:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@SUB_RES\n");
						fprintf(stdout, "DEC LF@SUB_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@SUB_RES int@0\n");
						fprintf(stdout, "DEC LF@SUB_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "SUB_RES"; 
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case MULTIPLY:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@MUL_RES\n");
						fprintf(stdout, "MUL LF@MUL_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@MUL_RES int@0\n");
						fprintf(stdout, "MUL LF@MUL_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "MUL_RES"; 
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case DIV:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@DIV_RES\n");
						fprintf(stdout, "DIV LF@DIV_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@DIV_RES int@0\n");
						fprintf(stdout, "DIV LF@DIV_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "DIV_RES"; 
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case MOD:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@IDIV_RES\n");
						fprintf(stdout, "IDIV LF@IDIV_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@IDIV_RES int@0\n");
						fprintf(stdout, "IDIV LF@IDIV_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "IDIV_RES"; 
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case HASH:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@HASH_RES\n");
						fprintf(stdout, "STRLEN LF@HASH_RES LF@%s\n", param1);
					}
					else{
						fprintf(stdout, "MOVE LF@HASH_RES int@0\n");
						fprintf(stdout, "STRLEN LF@HASH_RES LF@%s\n", param1);
					}
					param1 = "HASH_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case DOTDOT:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@DOTDOT_RES\n");
						fprintf(stdout, "CONCAT LF@DOTDOT LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@DOTDOT_RES int@0\n");
						fprintf(stdout, "CONCAT LF@DOTDOT LF@%s LF@%s\n", param1, param2);
					}
					param1 = "DOTDOT_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					break;
				case LESS:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@LESS_RES\n");
						fprintf(stdout, "LT LF@LESS_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@LESS_RES int@0\n");
						fprintf(stdout, "LT LF@LESS_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "LESS_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
                case MORE:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@MORE_RES\n");
						fprintf(stdout, "GT LF@MORE_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@MORE_RES int@0\n");
						fprintf(stdout, "GT LF@MORE_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "MORE_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
				case EQUAL:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@EQUAL_RES\n");
						fprintf(stdout, "EQ LF@EQUAL_RES LF@%s LF@%s\n", param1, param2);
					}
					else{
						fprintf(stdout, "MOVE LF@EQUAL_RES int@0\n");
						fprintf(stdout, "EQ LF@EQUAL_RES LF@%s LF@%s\n", param1, param2);
					}
					param1 = "EQUAL_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
				case LESSOREQUAL:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@LESSOREQUAL\n");
						fprintf(stdout, "DEFVAR LF@LESSOREQUAL1\n");
						fprintf(stdout, "DEFVAR LF@LESSOREQUAL_RES\n");
						fprintf(stdout, "LT LF@LESSOREQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@LESSOREQUAL1 LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "OR LF@LESSOREQUAL_RES LF@LESSOREQUAL LF@LESSOREQUAL1\n");
					}
					else{
						fprintf(stdout, "MOVE LF@LESSOREQUAL int@0\n");
						fprintf(stdout, "MOVE LF@LESSOREQUAL1 int@0\n");
						fprintf(stdout, "MOVE LF@LESSOREQUAL_RES int@0\n");
						fprintf(stdout, "LT LF@LESSOREQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@LESSOREQUAL1 LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "OR LF@LESSOREQUAL_RES LF@LESSOREQUAL LF@LESSOREQUAL1\n");
					}
					param1 = "LESSOREQUAL_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
				case MOREOREQUAL:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@MOREOREQUAL\n");
						fprintf(stdout, "DEFVAR LF@MOREOREQUAL1\n");
						fprintf(stdout, "DEFVAR LF@MOREOREQUAL_RES\n");
						fprintf(stdout, "GT LF@MOREOREQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@MOREOREQUAL1 LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "OR LF@MOREOREQUAL_RES LF@MOREOREQUAL LF@MOREOREQUAL1\n");
					}
					else{
						fprintf(stdout, "MOVE LF@MOREOREQUAL int@0\n");
						fprintf(stdout, "MOVE LF@MOREOREQUAL1 int@0\n");
						fprintf(stdout, "MOVE LF@MOREOREQUAL_RES int@0\n");
						fprintf(stdout, "GT LF@MOREOREQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@MOREOREQUAL1 LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "OR LF@MOREOREQUAL_RES LF@MOREOREQUAL LF@MOREOREQUAL1\n");
					}
					param1 = "MOREOREQUAL_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
				case NOTEQUAL:
					if(!(checkSEEN(oper))){
						fprintf(stdout, "DEFVAR LF@NOTEQUAL\n");
						fprintf(stdout, "DEFVAR LF@NOTEQUAL_RES\n");
						fprintf(stdout, "EQ LF@NOTEQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@NOTEQUAL_RES LF@NOTEQUAL1\n");
					}
					else{
						fprintf(stdout, "MOVE LF@NOTEQUAL int@0\n");
						fprintf(stdout, "MOVE LF@NOTEQUAL_RES int@0\n");
						fprintf(stdout, "EQ LF@NOTEQUAL LF@%s LF@%s\n", param1, param2);
						fprintf(stdout, "EQ LF@NOTEQUAL_RES LF@NOTEQUAL1\n");
					}
					param1 = "NOTEQUAL_RES";
					param2 = NULL;
					oper = 0;
					counter = 1;
					tmp = true;
					break;
			}
		}
	}
	if(!(ifSpotted(4)) && !(whileSpotted(4))){
		if(end && type != 38 && param2 == NULL){
			if(var_name != NULL){
				fprintf(stdout, "MOVE LF@%s ", var_name);
			}
			else{
				if(checkSEEN(1)){
					counter1 = 1;
				}
				fprintf(stdout, "MOVE LF@ret%d ", counter1);
				counter1++;
			}
			GEN_WRITE_VAR_LITERAL(type, param1);
			counter = 0;
		}
		else if(type == 38){
			counter = 0;
			type = 0;
			oper = 0;
		}
		else if(end && oper != 0){
			counter = 0;
		}
	}
	else if((ifSpotted(4) || whileSpotted(4)) && tmp){
		if(ifSpotted(4)){
			fprintf(stdout, "JUMPIFNEQ else %s bool@true\n", param1);
			ifSpotted(0);
			counter = 0;
			type = 0;
			oper = 0;
			tmp = false;
		}
		else if(whileSpotted(4)){
			fprintf(stdout, "LABEL while\n");
			fprintf(stdout, "JUMPIFEQ end %s bool@true\n", param1);
			counter = 0;
			type = 0;
			oper = 0;
			tmp = false;
		}
	}
}

//INBUILDS FUNCTION

void GENERATION_READI(){
fprintf(stdout, "LABEL $READI\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@ret1\n");
    fprintf(stdout, "DEFVAR LF@ret2\n");
    fprintf(stdout, "READ LF@ret1 int\n");
    fprintf(stdout, "DEFVAR LF@ret_check\n");

    fprintf(stdout, "TYPE LF@ret_check LF@ret1\n");
    fprintf(stdout, "JUMPIFNEQ $READI_END LF@ret_check string@int\n");

    fprintf(stdout, "PUSHS LF@ret1\n");
    fprintf(stdout, "MOVE LF@ret2 int@0\n");
    fprintf(stdout, "PUSHS LF@ret2\n");
    fprintf(stdout, "JUMP $END_READI\n");

	fprintf(stdout, "LABEL $READI_END\n");
    fprintf(stdout, "PUSHS LF@ret1\n");
    fprintf(stdout, "MOVE LF@ret2 int@1\n");
    fprintf(stdout, "PUSHS LF@ret2\n");

    fprintf(stdout, "LABEL $END_READI\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");
}

void GENERATION_READS(){
    fprintf(stdout, "LABEL $READS\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@ret1\n");
    fprintf(stdout, "DEFVAR LF@ret2\n");
    fprintf(stdout, "READ LF@ret1 string\n");
    fprintf(stdout, "DEFVAR LF@ret_check\n");

    fprintf(stdout, "TYPE LF@ret_check LF@ret1\n");
    fprintf(stdout, "JUMPIFNEQ $READS_END LF@ret_check string@string\n");

    fprintf(stdout, "PUSHS LF@ret1\n");
    fprintf(stdout, "MOVE LF@ret2 int@0\n");
    fprintf(stdout, "PUSHS LF@ret2\n");
    fprintf(stdout, "JUMP $END_READS\n");

	fprintf(stdout, "LABEL $READS_END\n");
    fprintf(stdout, "PUSHS LF@ret1\n");
    fprintf(stdout, "MOVE LF@ret2 int@1\n");
    fprintf(stdout, "PUSHS LF@ret2\n");

    fprintf(stdout, "LABEL $END_READS\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");
}

void GENERATION_READN(){
    fprintf(stdout, "LABEL $READN\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@ret\n");
    fprintf(stdout, "DEFVAR LF@ret2\n");
    fprintf(stdout, "DEFVAR LF@ret_check\n");
    fprintf(stdout, "READ LF@ret1 float\n");

    fprintf(stdout, "TYPE LF@ret_check LF@ret1\n");
    fprintf(stdout, "JUMPIFNEQ $INPUTF_END LF@ret_check string@float\n");

    fprintf(stdout, "PUSHS LF@ret\n");
    fprintf(stdout, "MOVE LF@ret2 int@0\n");
    fprintf(stdout, "PUSHS LF@ret2\n");
    fprintf(stdout, "JUMP $END_READN\n");

	fprintf(stdout, "LABEL $READN_END\n");
    fprintf(stdout, "MOVE LF@ret nil@nil\n");
    fprintf(stdout, "PUSHS LF@ret\n");
    fprintf(stdout, "MOVE LF@ret2 int@1\n");
    fprintf(stdout, "PUSHS LF@ret2\n");

    fprintf(stdout, "LABEL $END_READN\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");
}

void GENERATION_TOINTEGER(){
	fprintf(stdout, "LABEL $TOINTEGER\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@ret1\n");
	fprintf(stdout, "DEFVAR LF@param\n");
	fprintf(stdout, "MOVE LF@param LF@f$0\n");//f
	fprintf(stdout, "FLOAT2INT LF@ret1 LF@param\n");
	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");
}

void GENERATION_SUBSTR(){
	fprintf(stdout, "LABEL $SUBSTR\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@ret1\n");//string
	fprintf(stdout, "DEFVAR LF@ret2\n");//int

	fprintf(stdout, "DEFVAR LF@string\n");
	fprintf(stdout, "DEFVAR LF@from\n");
	fprintf(stdout, "DEFVAR LF@length_of_str\n");
	fprintf(stdout, "DEFVAR LF@length\n");

	fprintf(stdout, "DEFVAR LF@length_helper\n");
	fprintf(stdout, "DEFVAR LF@char\n");
	fprintf(stdout, "DEFVAR LF@new_strlen\n");

	fprintf(stdout, "MOVE LF@ret1 string@\n");
	fprintf(stdout, "MOVE LF@string LF@s$0\n");
	fprintf(stdout, "MOVE LF@from LF@i$0\n");
	fprintf(stdout, "MOVE LF@length_of_str LF@n$0\n");

	fprintf(stdout, "MOVE LF@length_helper int@0\n");

	fprintf(stdout, "STRLEN LF@length LF@string\n");//length = STRLEN(STRING)
	fprintf(stdout, "SUB LF@new_strlen LF@length int@1\n");//new_strlen = length - 1

	fprintf(stdout, "DEFVAR LF@result\n");
	fprintf(stdout, "LT LF@result LF@length_of_str int@0\n"); //n < O
	fprintf(stdout, "JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	fprintf(stdout, "EQ LF@result LF@length_of_str int@0\n"); //n == O
	fprintf(stdout, "JUMPIFEQ $SUBSTR_EMPTY LF@result bool@true\n");

	fprintf(stdout, "LT LF@result LF@from int@0\n"); //i < O
	fprintf(stdout, "JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	fprintf(stdout, "GT LF@result LF@from LF@new_strlen\n");//i >= length - 1
	fprintf(stdout, "JUMPIFEQ $SUBSTR_END LF@result bool@true\n");

	fprintf(stdout, "ADD LF@length_helper LF@length_helper LF@from\n");
	fprintf(stdout, "ADD LF@length_helper LF@length_helper LF@length_of_str\n");//i + n

	fprintf(stdout, "GT LF@result LF@length_helper LF@length\n");//i + n > n ? n : i+n
	fprintf(stdout, "JUMPIFEQ $SUBSTR_LEN LF@result bool@true\n");
	fprintf(stdout, "JUMP $FOR_LOOP\n");
	fprintf(stdout, "LABEL $SUBSTR_LEN\n");
	fprintf(stdout, "MOVE LF@length_helper LF@length\n");

	fprintf(stdout, "LABEL $FOR_LOOP\n");
	fprintf(stdout, "JUMPIFEQ $SUBSTR_RET_0 LF@length_helper LF@from\n");
	fprintf(stdout, "GETCHAR LF@char LF@string LF@from\n");
	fprintf(stdout, "CONCAT LF@ret1 LF@ret1 LF@char\n");
	fprintf(stdout, "ADD LF@from LF@from int@1\n");
	fprintf(stdout, "JUMP $FOR_LOOP\n");

	fprintf(stdout, "LABEL $SUBSTR_RET_0\n");
	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "MOVE LF@ret2 int@0\n");
	fprintf(stdout, "PUSHS LF@ret2\n");
	fprintf(stdout, "JUMP $END\n");

	fprintf(stdout, "LABEL $SUBSTR_END\n");
	fprintf(stdout, "MOVE LF@ret1 nil@nil\n");
	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "MOVE LF@ret2 int@1\n");
	fprintf(stdout, "PUSHS LF@ret2\n");

	fprintf(stdout, "LABEL $END\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");

	fprintf(stdout, "LABEL $SUBSTR_EMPTY\n");
	fprintf(stdout, "MOVE LF@ret1 string@\n");
	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "MOVE LF@ret2 int@0\n");
	fprintf(stdout, "PUSHS LF@ret2\n");
	fprintf(stdout, "JUMP $END\n\n");
}

void GENERATION_ORD(){
	fprintf(stdout, "LABEL $ORD\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@string\n");
	fprintf(stdout, "DEFVAR LF@int\n");
	fprintf(stdout, "DEFVAR LF@length\n");
	fprintf(stdout, "DEFVAR LF@right_int\n");
	fprintf(stdout, "DEFVAR LF@ret1\n");//string
	fprintf(stdout, "DEFVAR LF@ret2\n");//int

	fprintf(stdout, "MOVE LF@string LF@s$0\n");
	fprintf(stdout, "MOVE LF@int LF@i$0\n\n");

	fprintf(stdout, "STRLEN LF@length LF@string\n");//5
	fprintf(stdout, "SUB LF@length LF@length int@1\n");//4

	fprintf(stdout, "GT LF@right_int LF@int LF@length\n");
	fprintf(stdout, "JUMPIFEQ $ORD_END LF@right_int bool@true\n");//i > len(n)-1

	fprintf(stdout, "LT LF@right_int LF@int int@0\n");
	fprintf(stdout, "JUMPIFEQ $ORD_END LF@right_int bool@true\n");//i < 0

	fprintf(stdout, "STRI2INT LF@ret1 LF@string LF@int\n");

	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "MOVE LF@ret2 int@0\n");
	fprintf(stdout, "PUSHS LF@ret2\n");
	fprintf(stdout, "JUMP $ORD_RET\n");

	fprintf(stdout, "LABEL $ORD_END\n");
	fprintf(stdout, "MOVE LF@ret1 nil@nil\n");
	fprintf(stdout, "PUSHS LF@ret1\n");
	fprintf(stdout, "MOVE LF@ret2 int@1\n");
	fprintf(stdout, "PUSHS LF@ret2\n");

	fprintf(stdout, "LABEL $ORD_RET\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n\n");
}

void GENERATION_CHR(){
fprintf(stdout, "LABEL $CHR\n");
fprintf(stdout, "PUSHFRAME\n");
fprintf(stdout, "DEFVAR LF@int\n");
fprintf(stdout, "DEFVAR LF@right_int\n");
fprintf(stdout, "DEFVAR LF@ret1\n");//string
fprintf(stdout, "DEFVAR LF@ret2\n");//int

fprintf(stdout, "MOVE LF@int LF@i$0\n");

fprintf(stdout, "LT LF@right_int LF@int int@0\n");
fprintf(stdout, "JUMPIFEQ $CHR_END LF@right_int bool@true\n");//i < 0

fprintf(stdout, "GT LF@right_int LF@int int@255\n");
fprintf(stdout, "JUMPIFEQ $CHR_END LF@right_int bool@true\n");//i > len(n)-1

fprintf(stdout, "INT2CHAR LF@ret1 LF@int\n");

fprintf(stdout, "PUSHS LF@ret1\n");
fprintf(stdout, "MOVE LF@ret2 int@0\n");
fprintf(stdout, "PUSHS LF@ret2\n");
fprintf(stdout, "JUMP $CHR_RET\n");

fprintf(stdout, "LABEL $CHR_END\n");

fprintf(stdout, "MOVE LF@ret1 nil@nil\n");
fprintf(stdout, "PUSHS LF@ret1\n");
fprintf(stdout, "MOVE LF@ret2 int@1\n");
fprintf(stdout, "PUSHS LF@ret2\n");

fprintf(stdout, "LABEL $CHR_RET\n");
fprintf(stdout, "POPFRAME\n");
fprintf(stdout, "RETURN\n\n");
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
//         fprintf(stdout, "LABEL $%s$for$%d\n", token->data, count);
//     } else if (if_condition){
//         fprintf(stdout, "LABEL $%s$if$%d$else\n", token->data, count);
//     } else 
//         fprintf(stdout, "LABEL $%s$if$%d$end\n", token->data, count);
// }


// LABEL while
// JUMPIFEQ end GF@counter string@aaaaaaaaaaaaa
// WRITE GF@counter
// WRITE string@\010
// CONCAT GF@counter GF@counter string@a
// JUMP while

// LABEL end
