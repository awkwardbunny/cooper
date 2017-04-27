%{
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<math.h>

int yyerror();
int yylex();
%}

%token TOKEOF

%%

blah: TOKEOF;

%%

int main(){
}

int yyerror(){

}
