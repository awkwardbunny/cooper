%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <incl.h>
#include <symtab.h>

int yylex();
%}

%union {
	struct string_literal s;
	struct number n;
	struct identifier id;
	unsigned char charlit;
}

%token TOKEOF IDENT CHARLIT STRING NUMBER INDSEL PLUSPLUS MINUSMINUS SHL SHR LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS TIMESEQ DIVEQ MODEQ PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INLINE INT LONG REGISTER RESTRICT RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE _BOOL _COMPLEX _IMAGINARY

%%

func: IDENT '(' ')' code_block;

code_block: '{' func_lines '}';

func_lines:	func_line
		  |	func_lines func_line
		  ;
 // Statements also go here
func_line:	decl
		 | code_block
		 ;

decl: INT ident_list ';';

ident_list:	IDENT
		  | ident_list ',' IDENT
		  ;

%%

int main(){
}

