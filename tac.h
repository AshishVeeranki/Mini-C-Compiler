#ifndef TAC_H
#define TAC_H

#include "asty.h"

enum tac_names{
	TAC_ID,
	TAC_NUM,
	TAC_LABEL
};

enum tac_operator{
	TAC_NULL,
	TAC_ADDOP,
	TAC_SUBOP,
	TAC_MULOP,
	TAC_DIVOP,
	TAC_ADDOPU,
	TAC_SUBOPU,
	TAC_ASSIGN,
	TAC_ARRAY_REF,
	TAC_ARRAY_REF_LVAL,
	TAC_LESS,
	TAC_LESS_EQUAL,
	TAC_GREATER,
	TAC_GREATER_EQUAL,
	TAC_EQUAL,
	TAC_NEQUAL,
	TAC_PRINT,
	TAC_GOTO,
	TAC_STOP
};

struct tac_operand
{
	int val;
	int tac_type;
 	int label;
	struct symbol *psym;
};

struct tac_node
{
    int label;
    int op_type;
    struct tac_operand *operand1;
    struct tac_operand *operand2;
    struct tac_operand *operand3;
};

struct tac_node *into_tac(struct ast_node *p);
void print_tac();

#endif

