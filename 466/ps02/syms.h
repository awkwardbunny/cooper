#ifndef syms
#define syms

#define NS_NAME
#define NS_TAGS
#define NS_MEMB
#define NS_LABL

struct scope {
	struct scope *parent;
	struct scope *child;
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
void new_sym(int, char *, int);
int *get_sym(char *, int);
void new_scope();
void exit_scope();
void print_table();

#endif
