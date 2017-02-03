#ifndef SYMBOL_H
#define SYMBOL_H

struct symbol {
             char *name;
             int asize;
	     int addr;
             struct symbol *next;
   };
struct symbol *identifier;
struct symbol *getsym(char *name);
struct symbol *putsym(char *name, int size);


void push();
void pop();
void print_symbol();

#endif
