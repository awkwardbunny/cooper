%debug
%{
#include<stdlib.h>
#include<stdio.h>
#include<stdarg.h>
#include<math.h>
#include "inc.h"
#include "syms.h"

#define YYDEBUG 1
int yylex();

// Symbol table

%}

%union {
	struct string_literal s;
	struct number n;
	struct identifier id;
	unsigned char charlit;
}

%type<n.i> NUMBER expr prec_1 prec_2 prec_3 prec_4 prec_5 prec_6 prec_7 prec_8 prec_9 prec_10 prec_11 prec_12 prec_13 prec_14
%type<id.name> ident_list IDENT

%token TOKEOF
%token IDENT
%token CHARLIT
%token STRING
%token NUMBER
%token INDSEL
%token PLUSPLUS
%token MINUSMINUS
%token SHL
%token SHR
%token LTEQ
%token GTEQ
%token EQEQ
%token NOTEQ
%token LOGAND
%token LOGOR
%token ELLIPSIS
%token TIMESEQ
%token DIVEQ
%token MODEQ
%token PLUSEQ
%token MINUSEQ
%token SHLEQ
%token SHREQ
%token ANDEQ
%token OREQ
%token XOREQ
%token AUTO
%token BREAK
%token CASE
%token CHAR
%token CONST %token CONTINUE
%token DEFAULT
%token DO
%token DOUBLE
%token ELSE
%token ENUM
%token EXTERN
%token FLOAT
%token FOR
%token GOTO
%token IF
%token INLINE
%token INT
%token LONG
%token REGISTER
%token RESTRICT
%token RETURN
%token SHORT
%token SIGNED
%token SIZEOF
%token STATIC
%token STRUCT
%token SWITCH
%token TYPEDEF
%token UNION
%token UNSIGNED
%token VOID
%token VOLATILE
%token WHILE
%token _BOOL
%token _COMPLEX
%token _IMAGINARY

%%

cont: valid
	| cont valid
	;

valid: declaration
	| func_decl
	| func
	;

declaration: INT ident_list ';' { new_sym(1, $2, 0); print_table(); }
	;

ident_list: IDENT
	| ident_list ',' IDENT
	;

func_decl: IDENT '(' ')' ';'
	;

func: IDENT '(' ')' code_block { printf("a valid function!\n"); }
	;

code_block: '{' func_lines '}'

func_line:
	| declaration
	| statement
	| code_block
	;

func_lines: func_line
	| func_lines func_line
	;

statement: ';'
	| expr ';' { printf("exprval=%d\n", $1); print_table(); }
	;

prec_1: IDENT {
				//print_table();
				int *i = get_sym(yylval.id.name, 1);
				if(i)
					$$ = i;
				else{
					$$ = 0;
					yyerror("Error: Identifier %s not defined\n", yylval.id.name);
				}
			}
	| NUMBER { $$ = yylval.n.i; }
	| CHARLIT { $$ = 0; }
	| STRING { $$ = 0; }
	| IDENT '[' expr ']' { yyerror("Error: unimplemented blah[blah]\n"); }
	| IDENT '(' args_list ')' { yyerror("Error: unimplemented blah(blah, blah)\n"); }
	| IDENT '(' ')' { yyerror("Error: unimplemented blah()\n"); }
	| '(' expr ')' { $$ = $2; }
	| IDENT '.' IDENT { yyerror("Error: unimplemented blah.blah\n"); }
	| IDENT INDSEL IDENT { yyerror("Error: unimplemented blah->blah\n"); }
	| prec_1 PLUSPLUS { $$ = $1 + 1; }
	| prec_1 MINUSMINUS { $$ = $1 - 1; }

args_list: types IDENT
	| args_list ',' args_list
	| args_list ',' ELLIPSIS
	;

prec_2: prec_1
	| PLUSPLUS prec_1 { $$ = $2 + 1; }
	| MINUSMINUS prec_1 { $$ = $2 - 1; }
	| SIZEOF prec_1 { yyerror("Error: unimplemented sizeof(blah)\n"); }
	| '~' prec_1 { $$ = $2; }
	| '!' prec_1 { $$ = $2; }
	| '+' prec_1 { $$ = $2; }
	| '-' prec_1 { $$ = $2; }
	| '&' prec_1 { $$ = $2; }
	| '*' prec_1 { $$ = $2; }
	;

prec_3: prec_2
	| '(' types ')' prec_3 { yyerror("Error: unimplemented (blah)blah\n"); }
	;

types: quals prim_types
	;

quals: UNSIGNED
	| AUTO
	| STATIC
	| VOLATILE
	;

prim_types: INT
	| LONG
	| SHORT
	;

prec_4: prec_3
	| prec_4 '*' prec_3 { $$ = $1 * $3; }
	| prec_4 '/' prec_3 { $$ = $1 / $3; }
	| prec_4 '%' prec_3 { $$ = $1 % $3; }
	;

prec_5: prec_4
	| prec_5 '+' prec_4	{ $$ = $1 + $3; }
	| prec_5 '-' prec_4	{ $$ = $1 - $3; }
	;

prec_6: prec_5
	| prec_6 SHL prec_5
	| prec_6 SHR prec_5
	;

prec_7: prec_6
	| prec_7 EQEQ prec_6
	| prec_7 NOTEQ prec_6
	;

prec_8: prec_7
	| prec_8 '&' prec_7
	;

prec_9: prec_8
	| prec_9 '^' prec_8
	;

prec_10: prec_9
	| prec_10 '|' prec_9
	;

prec_11: prec_10
	| prec_11 LOGAND prec_10
	;

prec_12: prec_11
	| prec_12 LOGOR prec_11
	;

prec_13: prec_12
	| prec_12 '?' expr ':' prec_13
	;

prec_14: prec_13
	| prec_13 '=' prec_14 { *(&$$) = $3; }
	| prec_13 PLUSEQ prec_14 { $$ = $1 += $3; }
	| prec_13 MINUSEQ prec_14 { $$ = $1 -= $3; }
	| prec_13 TIMESEQ prec_14
	| prec_13 DIVEQ prec_14
	| prec_13 MODEQ prec_14
	| prec_13 SHLEQ prec_14
	| prec_13 SHREQ prec_14
	| prec_13 ANDEQ prec_14
	| prec_13 OREQ prec_14
	| prec_13 XOREQ prec_14
	;

expr: prec_14
	| expr ',' prec_14 { $$ = $3; }
	;

%%

int yydebug = 0;

int main(){
	init_sym_table();
	print_table();

	return yyparse();
}

void yyerror(char *s, ...){
	va_list ap;
	va_start(ap, s);

	fprintf(stderr, "%d: error: ", yylineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
}





















