%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "asty.h"
  #include "symbol.h"
  #include "tac.h"
  #include "mc.h"
    static struct ast_node *root;
    char *varname;
%}
  %union {
       int ival;
     struct ast_node *pnode;
   }
   %token <pnode> NUMBER
   %token <pnode> ID
   %token GE LE GT LT EQ NE IF WHILE DO ENDDO ENDIF PRINT THEN ELSE INT
   %type <pnode> program block vardec slist stmt assignst printstmt whilestmt expr term bexpr factor ifstmt
   %type <ival> relop addingop mulop
%%
   program:block { root = $1; }
       ;

block:  '{' { push(); } vardef slist '}' { pop(); } { $$ = new_ast_node(AST_BLOCK); $$->child1 = $4; }
     |  '{' { push(); } slist '}' { pop(); } { $$ = new_ast_node(AST_BLOCK); $$->child1 = $3; }
     ;

vardef:INT vardefl
      ;

vardefl:vardec
      |vardec ',' vardefl
      ;

vardec:ID { $1->psym = putsym(varname, 0); }

|ID '[' NUMBER ']' { $1->psym = putsym(varname, $3->ival); }
      ;

slist:stmt ';' slist   { if ($1) {
                                    $1->sibling = $3;
                                  }
                              else {
                                      $$ = $3;
                                   }
                     } 
      |stmt
      ;

stmt: assignst 
     | ifstmt
     |whilestmt
     |block
     |printstmt
     | /* empty */ { $$ = 0; }
  ;

assignst:ID '=' expr    { $$= new_ast_node(AST_ASSIGN); $$->child1=$1; $$->child2=$3;}

	|ID '[' expr ']' '=' expr
         {
             struct ast_node *t = new_ast_node(AST_ARRAY_REF_LVAL);
             t->child1 = $1;
             t->child2 = $3;
             $$ = new_ast_node(AST_ASSIGN);
             $$->child1=t;
             $$->child2=$6;
         } 
        ;

printstmt:PRINT'(' expr ')' { $$ = new_ast_node(AST_PRINT); $$->child1 = $3; }
	 ;

whilestmt:WHILE bexpr DO slist ENDDO  { $$= new_ast_node(AST_WHILE); $$->child1=$2; $$->child2=$4; }
	 ;

expr:expr addingop term     { if ($2 == 1) {
                                $$= new_ast_node(AST_ADDOP); }
	                      else {
                                $$= new_ast_node(AST_SUBOP);
                              }
                              $$->child1=$1; $$->child2=$3; }
         |term                { $$ = $1; }
         | addingop term   {  if ($1 == 1) {
                                $$= new_ast_node(AST_ADDOPU); }
	                      else {
                                $$= new_ast_node(AST_SUBOPU);
                              }  $$->child1=$2; }
         ;


addingop: '+'{$$=1;}	
	| '-'{$$=2;}
        ;

term : term mulop factor   {if($2==1) { $$= new_ast_node(AST_MULOP);}
                            else { $$=new_ast_node(AST_DIVOP);
                              } $$->child1=$1; $$->child2=$3; }
       |factor
       ;

mulop: '*'{$$=1;}
     |'/'{$$=2;}
      ;


factor : '(' expr ')'  { $$= $2; }
   |NUMBER
   |ID 
| ID '[' expr ']'{
    $$=new_ast_node(AST_ARRAY_REF);
    $$->child1=$1;
    $$->child2=$3;}
   ;


bexpr : expr relop expr   { $$= new_ast_node(AST_BEXPR); $$->relop = $2; $$->child1=$1; $$->child2=$3; }
      ;


relop : LT {$$=0;}   
      |GT {$$=1;}
      |LE {$$=2;}
      |GE {$$=3;}
      |EQ {$$=4;}
      |NE {$$=5;}	  
      ;
ifstmt : IF bexpr THEN slist ELSE  slist ENDIF { $$= new_ast_node(AST_IF); $$->child1=$2; $$->child2=$4;
                                                                                                       $$->child3=$6; }
       | IF bexpr THEN slist ENDIF { $$= new_ast_node(AST_IF); $$->child1=$2; $$->child2=$4; }
       ;
%%
#include "minic.lex.c" 


 main(int ac, char *av[])
{
    extern struct tac_node *prog;
    struct instr *mc_prog;

    if (ac < 2) {
        fprintf(stderr, "usage: a.exe file\n");
        exit(-1);
    }
    yyin = fopen(av[1], "r");
    if (yyin == 0) {
        fprintf(stderr, "could not open file %s\n", av[1]);
        exit(-1);
    }
    printf("calling yyparse\n");
    yyparse();
    print_ast(root);
    printf("symbol table is\n");
    print_symbol();
    printf("end");
    struct tac_node *prog;

    prog = into_tac(root);
    printf("the tac instructions are \n");
    print_tac();
    printf("the mc instructions are \n");
    mc_prog=into_mc(prog);
    print_mc(mc_prog);
}
       
