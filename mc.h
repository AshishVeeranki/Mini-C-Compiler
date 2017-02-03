#ifndef MC_H
#define MC_H

#include <stdio.h>
#include <stdlib.h>

#include "tac.h"
#include "asty.h"

struct asm_node
{
    int atype;
    int aval;
};

struct instr
{
    int label;
    int operator;
    struct asm_node *asm1;
    struct asm_node *asm2;
};

enum mc_oper{
    MC_NULL,
    MC_LOAD,
    MC_STORE,
    MC_OUT,
    MC_NEG,
    MC_ADD,
    MC_SUB,
    MC_MUL,
    MC_DIV,
    MC_JMP,
    MC_JEQ,
    MC_JNE,
    MC_JGE,
    MC_JGT,
    MC_JLE,
    MC_JLT,
    MC_NOP,
    MC_STOP
};

enum {
      ADDR,
      CONST,
      REG,
      LABEL,
      IADDR
};


struct instr *into_mc(struct tac_node *prog);

void print_mc(struct instr *st);

#endif
