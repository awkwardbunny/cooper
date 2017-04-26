#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "syms.h"

void init_sym_table(){
	global = (struct scope *)malloc(sizeof(struct scope));
	global->parent = NULL;
	global->child = NULL;

	//Dummy node (header)
	global->table = (struct sym *)malloc(sizeof(struct sym));
	global->table->ns = 0;
	global->table->name = -1;

	current = global;
}

void new_sym(int namesp, char *n, int val){
	printf("Adding symbol: %d %s\t%d\n", namesp, n, val);
	struct sym *i;
	for(i = current->table; i->next; i = i->next);
	i->next = (struct sym *)malloc(sizeof(struct sym));
	i = i->next;

	i->ns = namesp;
	i->name = (char *)malloc(strlen(n));
	strcpy(i->name, n);
	i->value = val;
	i->next = NULL;
}

int get_sym(char *n, int ns){
	for(struct scope *cs = current; cs; cs = cs->parent){
		printf(":%x\n", cs);
		for(struct sym *csym = cs->table->next; csym; csym = csym->next){
			printf("::%s\n", csym->name);
			if(ns == csym->ns && !strcmp(n, csym->name))
				return csym->value;
		}
	}
	return 0;
}

int set_sym(char *n, int ns, int v){
	for(struct scope *cs = current; cs; cs = cs->parent){
		printf(":%x\n", cs);
		for(struct sym *csym = cs->table->next; csym; csym = csym->next){
			printf("::%s\n", csym->name);
			if(ns == csym->ns && !strcmp(n, csym->name))
				csym->value = v;
		}
	}
	return -1;
}

void new_scope(){
	current->child = (struct scope *)malloc(sizeof(struct scope));
	current->child->parent = current;
	current = current->child;
	current->child = NULL;
	current->table = NULL;
}

void exit_scope(){
	struct sym *cs = current->table;
	while(cs){
		struct sym *ns = cs->next;
		free(cs);
		cs = ns;
	}

	current = current->parent;
	free(current->child);
}

void print_table(){
	printf("Symbols table:\n");
	struct sym *cs = current->table->next;
	while(cs){
		printf("%d: %s\t%d\n", cs->ns, cs->name, cs->value);
		cs = cs->next;
	}
	printf("----END OF TABLE----\n");
}
