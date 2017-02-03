
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "symbol.h"

struct table
{
	struct symbol *head;
	struct table *next;
};

static struct table *symlist_head=0;
static int naddr=1;


struct symbol *putsym(char *name,int size)
{
	struct table *symtab = symlist_head;
   	struct symbol *sym;
   	sym=(struct symbol*) malloc (sizeof(struct symbol));
	sym->name=strdup(name);
        sym->asize=size;
	sym->addr = naddr;
        if(size>0)
        { 
		naddr += size;
        }
	else
	{
		naddr += 1;
	}
       	sym->next=symtab->head;
	symtab->head=sym;
	return sym;
};


struct symbol *getsym(char *name)
{
   
	struct table *symtab= symlist_head;
	struct symbol *sym=0;
   	while(symtab) 
   	{	
       		sym= symtab->head;
       	while(sym)
       	{
	       if(!strcmp(sym->name,name))
	       {
                   printf("in get_sym: symbol %s found\n", name);
		       return sym;
	 	}
	       sym=sym->next;
       	}
       		symtab=symtab->next;
   	}
        printf("in get_sym: symbol %s not found\n", name);
   	return 0;
}

void push()
{
	struct table *symtab=malloc(sizeof(struct table));
	symtab->head=0;
	symtab->next=symlist_head;
	symlist_head=symtab;
}

void pop()
{
	struct table *symtab=symlist_head;
	symlist_head= symlist_head->next;
	free(symtab);
}

void print_symbol()
{
	struct table *symtab= symlist_head;
	 struct symbol *sym = 0;
    	while (symtab) 
    	{
        	printf("Symbol Table\n");
        	sym = symtab->head;
        	while (sym) 
		{
        	    	printf("name = %s \t asize = %d \t addr= %d\n", sym->name, sym->asize,sym->addr);
            		sym = sym->next;
        	}
        	symtab = symtab->next;
    	}
}

