#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <symtab.h>

/* TODO Should char * be copied over to a malloc'd buffer? */

void init_sym_table(){
	global = (struct scope *)malloc(sizeof(struct scope));
	global->name = "GLOBAL";
	global->parent = NULL;
	global->child = NULL;
	current = global;
}

void new_sym(char *n, int ns, int val){
	printf("Adding symbol: %d %s\t%d\n", ns, n, val);
	struct sym **spp;
	for(spp = &(current->table); *spp; spp = &((*spp)->next)){
		if(!strcmp((*spp)->name, n)){
			yyerror("Identifier already declared! Ignoring.\n");
			return;
		}
	}
	*spp = (struct sym *)malloc(sizeof(struct sym));
	//spp = &((*spp)->next);

	int nl = strlen(n);
	(*spp)->name = (char *)malloc(nl);
	(*spp)->ns = ns;
	strncpy((*spp)->name, n, nl);
	(*spp)->value = val;
	(*spp)->next = NULL;
}

int *get_sym(char *n, int ns){
	for(struct scope *csc = current; csc; csc = csc->parent){
		//printf(":%x\n", csc);
		for(struct sym *cs = csc->table; cs; cs = cs->next){
			//printf("::%s\n", csym->name);
			if(ns == cs->ns && !strcmp(n, cs->name))
				return &(cs->value);
		}
	}
	return 0;
}

/*
int set_sym(char *n, int ns, int v){
	for(struct scope *csc = current; csc; csc = csc->parent){
		printf(":%x\n", cs);
		for(struct sym *csym = cs->table->next; csym; csym = csym->next){
			printf("::%s\n", csym->name);
			if(ns == csym->ns && !strcmp(n, csym->name))
				csym->value = v;
		}
	}
	return -1;
}
*/

void new_scope(char *sname){
	current->child = (struct scope *)malloc(sizeof(struct scope));
	current->child->parent = current;
	current = current->child;

	current->name = sname;
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

	if(current==global){
		yyerror("Currently in global scope. Cannot exit. Ignoring.\n");
		return;
	}

	current = current->parent;
	free(current->child);
	current->child = NULL;
}

void print_table(){
	printf("Symbols table:\n");
	struct scope *csc;
	struct sym *cs;
	for(csc = global; csc; csc = csc->child){
		printf("%s: ", csc->name);
		for(cs = csc->table; cs; cs = cs->next){
			printf("(%s:%d:%s) ", cs->name, cs->value, (cs->ns ==NS_NAME)?"NAME":(cs->ns==NS_TAGS)?"TAG":(cs->ns==NS_MEMB)?"MEMB":"LABEL");
		}
		printf("\n");
	}
	printf("----END OF TABLE----\n");
}
