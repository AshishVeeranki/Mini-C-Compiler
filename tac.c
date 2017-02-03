#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "symbol.h"
#include "asty.h"
#include "tac.h"

int new_instr(int op_type,
              struct tac_operand *operand1,
              struct tac_operand *operand2,
              struct tac_operand *operand3);
struct tac_operand *convert_expr(struct ast_node *p);
struct tac_operand *convert_array_ref(struct ast_node *p);
struct tac_operand *convert_number(struct ast_node *p);
struct tac_operand *create_temp_operand();
struct tac_operand *create_num_operand();
struct tac_operand *convert_id_node(struct ast_node *P);
struct tac_operand *create_label(int label);
void convert_assign(struct ast_node *p);
void convert_print(struct ast_node *p);
void convert_while(struct ast_node *p);
void convert_if(struct ast_node *p);
int convert_bexpr(struct ast_node *p);
void convert_stmt(struct ast_node *p);
struct tac_node *into_tac(struct ast_node *p);
void print_tac_operand(struct tac_operand *operand);
void backpatch(int instr,struct tac_operand *operand); 


static struct tac_node prog[1024];
static int cur = 0;

extern char *ast_names[];

static char *operator_names[]={
	"TAC_NULL",
	"TAC_ADDOP",
	"TAC_SUBOP",
	"TAC_MULOP",
	"TAC_DIVOP",
	"TAC_ADDOPU",
	"TAC_SUBOPU",
	"TAC_ASSIGN",
	"TAC_ARRAY_REF",
	"TAC_ARRAY_REF_LVAL",
	"TAC_LESS",
	"TAC_LESS_EQUAL",
	"TAC_GREATER",
	"TAC_GREATER_EQUAL",
	"TAC_EQUAL",
	"TAC_NEQUAL",
	"TAC_PRINT",
	"TAC_GOTO",
	"TAC_STOP"
};

char *next_tmp_id()
{
    static char temp[8];
    static int temp_num = 1;
    sprintf(temp, "t%d", temp_num++);
    return (temp);
}



struct tac_operand* create_temp_operand()
{
    struct tac_operand *operand = malloc( sizeof(struct tac_operand));
    operand->tac_type = TAC_ID;
    char *temp = strdup(next_tmp_id());
    operand->psym = putsym(temp, 0);
    return (operand);
}

struct tac_operand *convert_expr(struct ast_node *p)
{
   
    if (p->ast_node_type == AST_ID) {
        return (convert_id_node(p));
    }
    if (p->ast_node_type == AST_NUMBER) {
        return (convert_number(p));
    }
    if (p->ast_node_type == AST_ARRAY_REF) {
        return (convert_array_ref(p));
    }
    int tac_operator = TAC_NULL;
    struct tac_operand *operand1 = 0;
    struct tac_operand *operand2 = 0;
    struct tac_operand *operand3 = 0;
    struct ast_node *child = 0;

    switch (p->ast_node_type) {
    case AST_SUBOPU:
        tac_operator = TAC_SUBOPU;
        break;
    case AST_ADDOP:
        tac_operator = TAC_ADDOP;
        break;
    case AST_SUBOP:
        tac_operator = TAC_SUBOP;
        break;
    case AST_MULOP:
        tac_operator = TAC_MULOP;
        break;
    case AST_DIVOP:
        tac_operator = TAC_DIVOP;
        break;
    case AST_ADDOPU:
	tac_operator = TAC_ADDOPU;
	break;
    default:
        tac_operator = TAC_NULL;
        break;
    }
    switch (p->ast_node_type) {
    case AST_NULL:
    case AST_ADDOPU:
        return (convert_expr(p->child1));
        break;
    case AST_SUBOPU:
        operand1 = convert_expr(p->child1);
        operand3 = create_temp_operand();
        break;
    case AST_ADDOP:
    case AST_SUBOP:
    case AST_MULOP:
    case AST_DIVOP:
        operand1 = convert_expr(p->child1);
        operand2 = convert_expr(p->child2);
        operand3 = create_temp_operand();
        break;
    }
    new_instr(tac_operator, operand1, operand2, operand3);
    return (operand3);
}


struct tac_operand *convert_id_node(struct ast_node *p)
{
    struct tac_operand *operand = malloc( sizeof(struct tac_operand));
    operand->tac_type = TAC_ID;
    operand->psym = p->psym;
    return (operand);
}

struct tac_operand* convert_array_ref(struct ast_node *p)
{
    struct tac_operand *operand1 = convert_id_node(p->child1);
    struct tac_operand *operand2 = convert_expr(p->child2);
    struct tac_operand *operand3 = create_temp_operand();
    new_instr(TAC_ARRAY_REF, operand1, operand2, operand3);
    return (operand3);
}

struct tac_operand* convert_number(struct ast_node *p)
{
    return (create_num_operand(p->ival));
}

struct tac_operand* create_num_operand(int value)
{
    struct tac_operand *operand = malloc( sizeof(struct tac_operand));
    operand->tac_type = TAC_NUM;
    operand->val = value;
    return (operand);
}


struct tac_operand *create_label(int label)
{
    struct tac_operand *operand = malloc( sizeof(struct tac_operand));
    operand->tac_type = TAC_LABEL;
    operand->label = label;
    return (operand);
}

void convert_assign(struct ast_node *p)
{
    struct tac_operand *operand1 = 0;
    struct tac_operand *operand2 = 0;
    struct tac_operand *operand3 = 0;
    if (p->child1->ast_node_type == AST_ID) {
        operand1 = convert_id_node(p->child1);
        operand2 = convert_expr(p->child2);
        new_instr(TAC_ASSIGN, operand1, operand2, 0);
    }
    else if (p->child1->ast_node_type == AST_ARRAY_REF_LVAL) {
        operand1 = convert_id_node(p->child1->child1);
        operand2 = convert_expr(p->child1->child2);
        operand3 = convert_expr(p->child2);
        new_instr(TAC_ARRAY_REF_LVAL, operand1, operand2, operand3);
    }
}

void convert_print(struct ast_node *p)
{
    struct tac_operand *operand1 = convert_expr(p->child1);
    new_instr(TAC_PRINT, operand1, 0, 0);
}

void convert_while(struct ast_node *p)
{
    int instr1 = convert_bexpr(p->child1);
    int instr2 = cur;
    new_instr(TAC_GOTO,0, 0, 0);
    int label1 = cur;
    convert_stmt(p->child2);
    struct tac_operand *operand3 = create_label(label1);
    backpatch(instr1,operand3);
    operand3 = create_label(instr1);
    new_instr(TAC_GOTO, operand3, 0, 0);
    int label2 = cur;
    operand3 = create_label(label2);
    backpatch(instr2,operand3);
}

int convert_bexpr(struct ast_node *p)
{
    int tac_operator = TAC_NULL;
    switch (p->relop) {
        case AST_LT:
        tac_operator = TAC_LESS;
        break;
    case AST_LE:
        tac_operator = TAC_LESS_EQUAL;
        break;
    case AST_GT:
        tac_operator = TAC_GREATER;
        break;
    case AST_GE:
        tac_operator = TAC_GREATER_EQUAL;
        break;
    case AST_EQ:
        tac_operator = TAC_EQUAL;
        break;
    case AST_NE:
        tac_operator = TAC_NEQUAL;
        break;
    default:
        break;
    }
    struct tac_operand *operand1 = convert_expr(p->child1);
    struct tac_operand *operand2 = convert_expr(p->child2);
    int instr1 = cur;
    new_instr(tac_operator, operand1, operand2, 0);
    return (instr1);
}
void convert_stmt(struct ast_node *p)
{
    while (p) {
        switch (p->ast_node_type) {
        case AST_ASSIGN:
            convert_assign(p);
            break;
        case AST_PRINT:
            convert_print(p);
            break;
        case AST_WHILE:
            convert_while(p);
            break;
        case AST_IF:
            convert_if(p);
            break;
        default:
            convert_stmt(p->child1);
            convert_stmt(p->child2);
            convert_stmt(p->child3);
            break;
        }
        p = p->sibling;
    }
}

struct tac_node *into_tac(struct ast_node *p)
{
    if (!p)
    {
         return;
    }
    push();
    convert_stmt(p);
    new_instr(TAC_STOP, 0, 0, 0);
    return prog;
}

void convert_if(struct ast_node *p)
{
    int instr1 = convert_bexpr(p->child1);
    int instr2 = cur;
    new_instr(TAC_GOTO, 0, 0, 0);
    int label1 = cur;
    convert_stmt(p->child2);
    struct tac_operand *operand3 = create_label(label1);
    backpatch(instr1,operand3);
    int instr3 = cur;
    new_instr(TAC_GOTO, 0, 0, 0);
    int label2 = cur;
    convert_stmt(p->child3);
    operand3 = create_label(label2);
    backpatch(instr2,operand3);
    int label3 = cur;
    operand3 = create_label(label3);
    backpatch(instr3,operand3);
}




int new_instr(int op_type, struct tac_operand *operand1,
                      struct tac_operand *operand2,
                      struct tac_operand *operand3)
{
    prog[cur].op_type = op_type;
    prog[cur].operand1 = operand1;
    prog[cur].operand2 = operand2;
    prog[cur].operand3 = operand3;
    return (cur++);
}

void backpatch(int instr,struct tac_operand *operand)
{
	if(prog[instr].op_type==TAC_GOTO)
        {
		prog[instr].operand1= operand;
	}
	else
		prog[instr].operand3=operand;

}

void print_tac()
{
    int i = 0;

    while (prog[i].op_type != TAC_NULL) {
        printf("%d : %s ", i, operator_names[prog[i].op_type]);
        print_tac_operand(prog[i].operand1);
        print_tac_operand(prog[i].operand2);
        print_tac_operand(prog[i].operand3);
        printf("\n");
        i++;
    }
}


void print_tac_operand(struct tac_operand *operand)
{
    if (!operand)
    {
        return;
    }
    switch(operand->tac_type) {
    case TAC_ID:
         printf("name = %s\t asize = %d \taddr=%d \n", operand->psym->name,
               operand->psym->asize,operand->psym->addr);
        break;
    case TAC_NUM:
        printf("value = %d ", operand->val);
        break;
    case TAC_LABEL:
        printf("label = %d ", operand->label);
        break;
    }
}


