#include "asty.h"

char *ast_names[] = 
{
    "AST_IF",
    "AST_WHILE",
    "+",
    "-",
    "AST_BEXPR",
    "+",
    "-",
    "*",
    "/",
    "=",
    "AST_ID",
    "AST_NUMBER",
    "AST_BLOCK",
    "AST_PRINT",
    "AST_ARRAY_REF",
    "AST_ARRAY_REF_LVAL",
    "AST_NULL"
};

static char *ast_oper[] = 
{
    "<",
    ">",
    ">=",
    "<=",
    "==",
    "!="
};

 struct ast_node *new_ast_node(int ast_node_type)
{
    printf("in new_ast_node: ast_node_type = %s\n", ast_names[ast_node_type]);
        struct ast_node *p=malloc(sizeof(struct ast_node));
	p->ast_node_type = ast_node_type;
        return p;
};

void add_sibling(struct ast_node *node, struct ast_node *sibling)
{
    while (node->sibling) {
        node = node->sibling;
    }
    node->sibling = sibling;
}


void print_ast( struct ast_node *p1)
{
    while(p1)
    {
        printf("node type=%s \n",ast_names[p1->ast_node_type]);
        switch(p1->ast_node_type)
        {
        case AST_BEXPR: 
            printf("relop=%s \n",ast_oper[p1->relop]);
            print_ast(p1->child1);
            print_ast(p1->child2);
            break;
        case AST_NUMBER:
            printf("number is=%d \n",p1->ival);
            break;
        case AST_ID:
            printf("identifier is =%s\n",p1->psym->name);
                   break;
        case AST_ARRAY_REF:
        case AST_ARRAY_REF_LVAL:
            printf("array is = %s size = %d \n",
                   p1->child1->psym->name, p1->child1->psym->asize);
            print_ast(p1->child2);
            break;
        default:
            print_ast(p1->child1);
            print_ast(p1->child2);
            print_ast(p1->child3);
            break;
        }
        p1 = p1->sibling;
    }
}



