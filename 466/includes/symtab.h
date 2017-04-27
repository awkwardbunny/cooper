#ifndef syms
#define syms
#include <stdarg.h>

#define NS_NAME 1
#define NS_TAGS 2
#define NS_MEMB 3
#define NS_LABL 4

struct scope {
	struct scope *parent;
	struct scope *child;
	char *name;
	struct sym *table;
};

struct sym {
	int ns;
	char *name;
	int value;
	struct sym *next;
};

struct scope *global, *current;

void init_sym_table();
void new_sym(char *, int, int);
int *get_sym(char *, int);
void new_scope(char *);
void exit_scope();
void print_table();

void yyerror(char *, ...);
#endif
