#ifndef ASTY_H
#define ASTY_H

#include <stdio.h>
#include <stdlib.h>

#include "symbol.h"
 
enum ast_type{
    AST_IF,
    AST_WHILE,
    AST_ADDOPU,
    AST_SUBOPU,
    AST_BEXPR,
    AST_ADDOP,
    AST_SUBOP,
    AST_MULOP,
    AST_DIVOP,
    AST_ASSIGN,
    AST_ID,
    AST_NUMBER,
    AST_BLOCK,
    AST_PRINT,
    AST_ARRAY_REF,
    AST_ARRAY_REF_LVAL,
    AST_NULL
};
enum ast_oper{
	AST_LT,
	AST_GT,
	AST_GE,
	AST_LE,
	AST_EQ,
	AST_NE
	};
struct ast_node
{
       int ast_node_type;
       int relop;
       struct ast_node *child1;
       struct ast_node *child2;
       struct ast_node *child3;
       struct ast_node *sibling;
    struct symbol *psym;
    int ival;
};

struct ast_node *new_ast_node(int ast_node_type);
  
void add_sibling(struct ast_node *node, struct ast_node *sibling);

#endif
