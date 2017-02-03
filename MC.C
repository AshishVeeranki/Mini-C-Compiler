#include <stdlib.h>
#include <stdio.h>

#include "symbol.h"
#include "mc.h"
#include "tac.h"

static struct instr prog_mc[1024];
static int cur = 0;

static char *operator_names[] = {
    "NULL",
    "LOAD",
    "STORE",
    "OUT",
    "NEG",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "JMP",
    "JEQ",
    "JNE",
    "JGE",
    "JGT",
    "JLE",
    "JLT",
    "NOP",
    "STOP"
};


struct asm_node *create_asm(int atype, int aval);
struct asm_node *convert_operand_to_asm(struct tac_operand *operand);
int new_mc_instr(int label, int operator, struct asm_node *asm1,struct asm_node *asm2);

void convert_assign_asm(int label, struct tac_node *p);
void convert_assign_array_ref_asm(int label, struct tac_node *p);
void convert_print_asm(int label, struct tac_node *p);
void convert_goto(int label, struct tac_node *p);
void convert_unary_minus(int label, struct tac_node *p);
void convert_relop(int label, struct tac_node *p);
void convert_binop(int label, struct tac_node *p);
void convert_array_ref_asm(int label, struct tac_node *p);
struct instr *into_mc(struct tac_node *prog);
void print_mc_asm(struct asm_node *asm3);
void print_mc(struct instr *mc_prog);


struct asm_node *create_asm(int atype, int aval)
{
    struct asm_node *asm1 = malloc( sizeof(struct asm_node));
    asm1->atype = atype;
    asm1->aval = aval;
    return asm1;
}

struct asm_node *convert_operand_to_asm(struct tac_operand *operand)
{
    struct asm_node *asm1 = malloc( sizeof(struct asm_node));
    int atype;
    int aval;
    if (operand->tac_type == TAC_ID) {
        atype = ADDR;
        aval = operand->psym->addr;
    }
    else if (operand->tac_type == TAC_NUM) {
        atype = CONST;
        aval = operand->val;
    }
    else if (operand->tac_type == TAC_LABEL) {
        atype = LABEL;
        aval = operand->label;
    }
    asm1->atype = atype;
    asm1->aval = aval;
    return asm1;
}

int new_mc_instr(int label, int operator, struct asm_node *asm1,
                     struct asm_node *asm2)
{
    prog_mc[cur].label = label;
    prog_mc[cur].operator = operator;
    prog_mc[cur].asm1 = asm1;
    prog_mc[cur].asm2 = asm2;
    return (cur++);
}

void convert_assign_asm(int label, struct tac_node *p)
{
    struct asm_node *asm1 = convert_operand_to_asm(p->operand2);
    struct asm_node *asm2 = create_asm(REG, 0);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = convert_operand_to_asm(p->operand1);
    new_mc_instr(-1, MC_STORE, asm2, asm1);
}

void convert_assign_array_ref_asm(int label, struct tac_node *p)
{
    int baddr = p->operand1->psym->addr;
    struct asm_node *asm1 = convert_operand_to_asm(p->operand2);
    struct asm_node *asm2 = create_asm(REG, 1);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = create_asm(CONST, baddr);
    asm2 = create_asm(REG, 1);
    new_mc_instr(-1, MC_ADD, asm1, asm2);

    asm1 = convert_operand_to_asm(p->operand3);
    asm2 = create_asm(REG, 0);
    new_mc_instr(-1, MC_LOAD, asm1, asm2);

    asm1 = create_asm(REG, 0);
    asm2 = create_asm(IADDR, 1);
    new_mc_instr(-1, MC_STORE, asm1, asm2);
}

void convert_print_asm(int label, struct tac_node *p)
{
    struct asm_node *asm1 = convert_operand_to_asm(p->operand1);
    struct asm_node *asm2 = create_asm(REG, 0);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    new_mc_instr(-1, MC_OUT, asm2, 0);
}

void convert_goto(int label, struct tac_node *p)
{
    struct asm_node *asm1 = convert_operand_to_asm(p->operand1);
    new_mc_instr(label, MC_JMP, asm1, 0);
}

void convert_unary_minus(int label, struct tac_node *p)
{
    struct asm_node *asm1 = convert_operand_to_asm(p->operand1);
    struct asm_node *asm2 = create_asm(REG, 0);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = create_asm(REG, 0);
    new_mc_instr(-1, MC_NEG, asm1, 0);
    asm1 = create_asm(REG, 0);
    asm2 = convert_operand_to_asm(p->operand3);
    new_mc_instr(-1, MC_STORE, asm1, asm2);
}

void convert_relop(int label, struct tac_node *p)
{
    int operator;
    switch (p->op_type) {
        case TAC_LESS:
            operator = MC_JLT;
            break;
        case TAC_LESS_EQUAL:
            operator = MC_JLE;
            break;
        case TAC_GREATER:
            operator = MC_JGT;
            break;
        case TAC_GREATER_EQUAL:
            operator = MC_JGE;
            break;
        case TAC_EQUAL:
            operator = MC_JEQ;
            break;
        case TAC_NEQUAL:
            operator = MC_JNE;
            break;
    }
    struct asm_node *asm1 = convert_operand_to_asm(p->operand1);
    struct asm_node *asm2 = create_asm(REG, 0);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = convert_operand_to_asm(p->operand2);
    asm2 = create_asm(REG, 0);
    new_mc_instr(-1, MC_SUB, asm1, asm2);
    asm1 = create_asm(REG, 0);
    asm2 = convert_operand_to_asm(p->operand3);
    new_mc_instr(-1, operator, asm1, asm2);
}

void convert_binop(int label, struct tac_node *p)
{
    int operator;
    switch (p->op_type) {
        case TAC_ADDOP:
            operator = MC_ADD;
            break;
        case TAC_SUBOP:
            operator = MC_SUB;
            break;
        case TAC_MULOP:
            operator = MC_MUL;
            break;
        case TAC_DIVOP:
            operator = MC_DIV;
            break;
    }
    struct asm_node *asm1 = convert_operand_to_asm(p->operand1);
    struct asm_node *asm2 = create_asm(REG, 0);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = convert_operand_to_asm(p->operand2);
    asm2 = create_asm(REG, 0);
    new_mc_instr(-1, operator, asm1, asm2);
    asm1 = create_asm(REG, 0);
    asm2 = convert_operand_to_asm(p->operand3);
    new_mc_instr(-1, MC_STORE, asm1, asm2);
}

void convert_array_ref_asm(int label, struct tac_node *p)
{
    int baddr = p->operand1->psym->addr;
    struct asm_node *asm1 = convert_operand_to_asm(p->operand2);
    struct asm_node *asm2 = create_asm(REG, 1);
    new_mc_instr(label, MC_LOAD, asm1, asm2);
    asm1 = create_asm(CONST, baddr);
    asm2 = create_asm(REG, 1);
    new_mc_instr(-1, MC_ADD, asm1, asm2);
    asm1 = create_asm(IADDR, 1);
    asm2 = create_asm(REG, 0);
    new_mc_instr(-1, MC_LOAD, asm1, asm2);
    asm1 = create_asm(REG, 0);
    asm2 = convert_operand_to_asm(p->operand3);
    new_mc_instr(-1, MC_STORE, asm1, asm2);
}

struct instr *into_mc(struct tac_node *prog)
{
    int i = 0;
    struct tac_node *p;
    while (prog[i].op_type != TAC_NULL) {
        p = &prog[i];
        switch (p->op_type) {
        case TAC_ASSIGN:
            convert_assign_asm(i, p);
            break;
        case TAC_ARRAY_REF:
            convert_array_ref_asm(i, p);
            break;
        case TAC_ADDOP:
        case TAC_SUBOP:
        case TAC_MULOP:
        case TAC_DIVOP:
            convert_binop(i, p);
            break;
        case TAC_LESS:
        case TAC_LESS_EQUAL:
        case TAC_GREATER:
        case TAC_GREATER_EQUAL:
        case TAC_EQUAL:
        case TAC_NEQUAL:
            convert_relop(i, p);
            break;
        case TAC_PRINT:
            convert_print_asm(i, p);
            break;
        case TAC_STOP:
            new_mc_instr(i, MC_STOP, 0, 0);
            break;
        case TAC_SUBOPU:
            convert_unary_minus(i, p);
            break;
        case TAC_GOTO:
            convert_goto(i, p);
            break;
        case TAC_ARRAY_REF_LVAL:
            convert_assign_array_ref_asm(i, p);
            break;
        default:
            break;
        }
        i++;
    }
    return (prog_mc);
}

void print_mc_asm(struct asm_node *asm3)
{
    if (!asm3) {
        return;
    }
    switch (asm3->atype) {
    case ADDR:
    case LABEL:
        printf("%d ", asm3->aval);
        break;
    case IADDR:
        printf("@r%d ", asm3->aval);
        break;
    case CONST:
        printf("#%d ", asm3->aval);
        break;
    case REG:
        printf("r%d ", asm3->aval);
        break;
    default:
        break;
    }
}

void print_mc(struct instr *mc_prog)
{
    int i;

    printf("\n\n");
    for (i = 0; i < cur; i++) {
        if (mc_prog[i].label >= 0) {
            printf("%d: %s ", mc_prog[i].label, operator_names[mc_prog[i].operator]);
        }
        else {
            printf("%s ", operator_names[mc_prog[i].operator]);
        }
        print_mc_asm(mc_prog[i].asm1);
        if (mc_prog[i].asm2) {
            printf(",");
            print_mc_asm(mc_prog[i].asm2);
        }
        printf("\n");
    }
}
