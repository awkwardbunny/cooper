%{
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

/** To be included in a separate header file **/
#include "ltests/tokens-manual.h"

struct string_literal {
	unsigned char *characters;
	int length;
};

#define NT_INT 2
#define NT_UINT 3
#define NT_L 4
#define NT_UL 5
#define NT_LL 6
#define NT_ULL 7
#define NT_FP 8
#define NT_DBL 9
#define NT_LDBL 10

struct number {
	char type;
	unsigned int i;
	unsigned long l;
	unsigned long long ll;
	float f;
	double d;
	long double ld;
};

struct identifier {
	char *name;
};

typedef union {
	struct string_literal s;
	struct number n;
	struct identifier id;
	unsigned char charlit;
} YYSTYPE;

YYSTYPE yylval;

int line;
char filename[64];

void setline(char *);
void mystrcat(char *str, int l);
void processint(char *, int, int);
int mode = 0;
%}

%x CHAR
%x STRING
%x ESCAPE
%x INT
%x FP
%%

 /* LINE NUMBERS */
#[ ][0-9]+[ ]\"[^ \"]+\"([ ][1-4])*\n { setline(yytext); }
\n                                    { line++; }

 /* WHITESPACES */
[\t\v ]+     { }

 /* KEYWORDS */
"auto"       { return AUTO; }
"break"      { return BREAK; }
"case"       { return CASE; }
"char"       { return CHAR; }
"const"      { return CONST; }
"continue"   { return CONTINUE; }
"default"    { return DEFAULT; }
"do"         { return DO; }
"double"     { return DOUBLE; }
"else"       { return ELSE; }
"enum"       { return ENUM; }
"extern"     { return EXTERN; }
"float"      { return FLOAT; }
"for"        { return FOR; }
"goto"       { return GOTO; }
"if"         { return IF; }
"inline"     { return INLINE; }
"int"        { return INT; }
"long"       { return LONG; }
"register"   { return REGISTER; }
"restrict"   { return RESTRICT; }
"return"     { return RETURN; }
"short"      { return SHORT; }
"signed"     { return SIGNED; }
"sizeof"     { return SIZEOF; }
"static"     { return STATIC; }
"struct"     { return STRUCT; }
"switch"     { return SWITCH; }
"typedef"    { return TYPEDEF; }
"union"      { return UNION; }
"unsigned"   { return UNSIGNED; }
"void"       { return VOID; }
"volatile"   { return VOLATILE; }
"while"      { return WHILE; }
"_Bool"      { return _BOOL; }
"_Complex"   { return _COMPLEX; }
"_Imaginary" { return _IMAGINARY; }

 /* SEPARATORS */
[()\[\]{},;:] { return yytext[0]; }
"..."        { return ELLIPSIS; }
  /* C99 DIGRAPHS */
"<:"         { return '['; }
":>"         { return ']'; }
"<%"         { return '{'; }
"%>"         { return '}'; }
"%:"         { return '#'; }

 /* OPERATORS */
 /** SIMPLE */
[!%^&*\-~+=|.<>\/?] { return yytext[0]; }
 /** COMPOUND */
"+="         { return PLUSEQ; }
"-="         { return MINUSEQ; }
"*="         { return TIMESEQ; }
"/="         { return DIVEQ; }
"%="         { return MODEQ; }
"<<="        { return SHLEQ; }
">>="        { return SHREQ; }
"&="         { return ANDEQ; }
"^="         { return XOREQ; }
"|="         { return OREQ; }
 /** OTHER */
"->"         { return INDSEL; }
"++"         { return PLUSPLUS; }
"--"         { return MINUSMINUS; }
"<<"         { return SHL; }
">>"         { return SHR; }
"<="         { return LTEQ; }
">="         { return GTEQ; }
"=="         { return EQEQ; }
"!="         { return NOTEQ; }
"&&"         { return LOGAND; }
"||"         { return LOGOR; }

 /* IDENTIFIERS */
[a-zA-Z_][0-9a-zA-Z_]* {
	yylval.id.name = malloc(yyleng);
	memcpy(yylval.id.name, yytext, yyleng);
	return IDENT; 
}

 /* CONSTANTS */
 /** INTEGER */
[1-9][0-9]*(((l|L|ll|LL)[uU]?)|[uU](l|L|ll|LL)?)? {
	processint(yytext, yyleng, 10);
	return NUMBER;
}

0[0-7]*(((l|L|ll|LL)[uU]?)|[uU](l|L|ll|LL)?)? {
	processint(yytext, yyleng, 8);
	return NUMBER;
}

0[xX][0-9a-fA-F]*(((l|L|ll|LL)[uU]?)|[uU](l|L|ll|LL)?)? {
	processint(yytext, yyleng, 16);
	return NUMBER;
}

 /** FLOAT */
(([0-9]+[eE]|0[xX]([0-9a-fA-F]+\.[0-9a-fA-F]*|\.?[0-9a-fA-F]+)[pP])[+-]?[0-9]+|(\.[0-9]+|[0-9]+\.[0-9]*)([eE][+-]?[0-9]+)?)[fFlL]? {
	switch(yytext[yyleng-1]){
		case 'f':
		case 'F':
			yytext[--yyleng] = '\0';
			yylval.n.f = strtof(yytext, NULL);
			if(yylval.n.f == HUGE_VAL)
				fprintf(stderr, "%s:%d:Warning:%s\n", filename, line, strerror(errno));
			yylval.n.type = NT_FP;
			break;
		case 'l':
		case 'L':
			yytext[--yyleng] = '\0';
			yylval.n.ld = strtold(yytext, NULL);
			if(yylval.n.ld == HUGE_VAL)
				fprintf(stderr, "%s:%d:Warning:%s\n", filename, line, strerror(errno));
			yylval.n.type = NT_LDBL;
			break;
		default:
			yylval.n.d = strtod(yytext, NULL);
			if(yylval.n.d == HUGE_VAL)
				fprintf(stderr, "%s:%d:Warning:%s\n", filename, line, strerror(errno));
			yylval.n.type = NT_DBL;
	}
	return NUMBER;
}

 /** CHAR */
L?\' {
	yylval.charlit = 0;
	mode = CHARLIT;
	BEGIN(CHAR);
}
<CHAR>[^\n\'\\]+ {
	if(!yylval.charlit)
		yylval.charlit = yytext[0];
	if(yyleng > 1)
		fprintf(stderr, "%s:%d:Warning:Unsupported multibyte character literal truncated to first byte\n", filename, line);
}
<CHAR>\\ {
	BEGIN(ESCAPE);
}
<CHAR>\n {
	fprintf(stderr, "%s:%d:Error:Unexpected newline\n", filename, line);
	exit(-1);
}
<CHAR>\' {
	BEGIN(0);
	mode = 0;
	return CHARLIT;
}

 /** STRING */
L?\" {
	yylval.s.length = 0;
	mode = STRING;
	BEGIN(STRING);
}
<STRING>[^\n\"\\]* {
	mystrcat(yytext, yyleng);
}
<STRING>\\ {
	BEGIN(ESCAPE);
}
<STRING>\n {
	fprintf(stderr, "%s:%d:Error:Unexpected newline\n", filename, line);
	exit(-1);
}
<STRING>\" {
	mystrcat(yytext+1, 1);
	yylval.s.length--;
	BEGIN(0);
	mode = 0;
	return STRING;
}

 /* ESCAPE CODES */
 /** CHARACTER */
<ESCAPE>[ntbrfv\\'"a?] {
	char x;
	switch(yytext[0]){
		case 'n': x = '\n'; break;
		case 't': x = '\t'; break;
		case 'b': x = '\b'; break;
		case 'r': x = '\r'; break;
		case 'f': x = '\f'; break;
		case 'v': x = '\v'; break;
		case '\\': x = '\\'; break;
		case '\'': x = '\''; break;
		case '"': x = '"'; break;
		case 'a': x = '\a'; break;
		case '?': x = '?'; break;
	}
	switch(mode){
		case STRING:
			mystrcat(&x, 1);
			BEGIN(STRING);
			break;
		case CHARLIT:
			yylval.charlit = x;
			BEGIN(CHAR);
	}
}
 /** OCTAL */
<ESCAPE>[0-7]{1,3} {
	long int l = strtol(yytext, NULL, 8);
	if(l == LONG_MIN || l == LONG_MAX){
		fprintf(stderr, "%s:%d:Warning:%s\n", filename, line, strerror(errno));
	}
	char o = (char)l;
	switch(mode){
		case STRING:
			mystrcat(&o, 1);
			BEGIN(STRING);
			break;
		case CHARLIT:
			yylval.charlit = o;
			BEGIN(CHAR);
	}
}

 /** HEX */
<ESCAPE>[xX][0-9a-fA-F]+ {
	long int l = strtol(yytext+1, NULL, 16);
	unsigned char h = (unsigned char)l;
	if(l > 0xff){
		fprintf(stderr, "%s:%d:Warning:Hex escape sequence \\%s out of range\n", filename, line, yytext);
		h = 0xff;
	}
	switch(mode){
		case STRING:
			mystrcat(&h, 1);
			BEGIN(STRING);
			break;
		case CHARLIT:
			yylval.charlit = h;
			BEGIN(CHAR);
			break;
	}
}

%%

void setline(char *yytext){
	sscanf(yytext, "# %d \"%62[^\"]+\"", &line, filename);
}

void mystrcat(char *str, int l){
	if(yylval.s.length){
		char *new = (char *)realloc(yylval.s.characters, yylval.s.length+l);
		if(!new){
			fprintf(stderr, "Error: realloc failed in mystrcat()! %s\n", strerror(errno));
			exit(-1);
		}
		yylval.s.characters = new;
	}else{
		yylval.s.characters = (char *)malloc(l);
	}
	memcpy(yylval.s.characters+yylval.s.length, str, l);
	yylval.s.length += l;
}

void printchar(unsigned char cin){
	char c;
	switch(cin){
		case '\0': c = '0'; goto p;
		case '\a': c = 'a'; goto p;
		case '\b': c = 'b'; goto p;
		case '\f': c = 'f'; goto p;
		case '\n': c = 'n'; goto p;
		case '\r': c = 'r'; goto p;
		case '\t': c = 't'; goto p;
		case '\v': c = 'v'; goto p;
		case '\'': c = '\''; goto p;
		case '"': c = '"'; goto p;
		case '\\': c = '\\';
		p: printf("\\%c", c); break;
		default: 
			if(isprint(cin)){
				printf("%c", cin);
			}else{
				printf("\\%03o", cin);
			}
	}

}

void processint(char *str, int len, int base){
	int uns = 0;
	int longs = 0;
	for(int i = 0; i < len; i++){
		switch(str[i]){
			case 'u':
			case 'U':
				uns++;
				str[i] = 0;
				break;
			case 'l':
			case 'L':
				longs++;
				str[i] = 0;
		}
	}
	
	long long int ll = strtoll(str, NULL, base);
	if(ll == LLONG_MIN || ll == LLONG_MAX){
		fprintf(stderr, "%s:%d:Warning:%s\n", filename, line, strerror(errno));
	}

	if(uns){
		if(longs == 1){
			yylval.n.type = NT_UL;
			yylval.n.l = ll;
		}else if(longs == 2){
			yylval.n.type = NT_ULL;
			yylval.n.ll = ll;
		}else{
			yylval.n.type = NT_UINT;
			yylval.n.i = ll;
		}
	}else{
		if(longs == 1){
			yylval.n.type = NT_L;
			yylval.n.l = ll;
		}else if(longs == 2){
			yylval.n.type = NT_LL;
			yylval.n.ll = ll;
		}else{
			yylval.n.type = NT_INT;
			yylval.n.i = ll;
		}

	}
}

int main(){
	line = 1;
	strcpy(filename, "<stdin>");
	int t;
	while(t = yylex()){
		printf("%s\t%d\t", filename, line);
		switch(t){
			case IDENT: printf("IDENT\t%s\n", yylval.id.name); break;
			case CHARLIT:
				printf("CHARLIT\t");
				printchar(yylval.charlit);
				printf("\n");
				break;
			case STRING :
				printf("STRING\t");
				for(int i = 0; i < yylval.s.length; i++){
					printchar(yylval.s.characters[i]);
				}
				printf("\n");
				break;
			case NUMBER :
				printf("NUMBER\t");
				switch(yylval.n.type){
					case NT_INT:
						printf("INTEGER\t%d\tINT\n", yylval.n.i); break;
					case NT_UINT:
						printf("INTEGER\t%u\tUNSIGNED,INT\n", yylval.n.i); break;
					case NT_L:
						printf("INTEGER\t%ld\tLONG\n", yylval.n.l); break;
					case NT_UL:
						printf("INTEGER\t%lu\tUNSIGNED,LONG\n", yylval.n.l); break;
					case NT_LL:
						printf("INTEGER\t%lld\tLONGLONG\n", yylval.n.ll); break;
					case NT_ULL:
						printf("INTEGER\t%llu\tUNSIGNED,LONGLONG\n", yylval.n.ll); break;
					case NT_FP:
						printf("REAL\t%g\tFLOAT\n", yylval.n.f); break;
					case NT_DBL:
						printf("REAL\t%g\tDOUBLE\n", yylval.n.d); break;
					case NT_LDBL:
						printf("REAL\t%Lg\tLONGDOUBLE\n", yylval.n.ld);
				}
				break;
			// Tabs needed to match ltests/ltests.output with diff
			case INDSEL :  printf("INDSEL\t\n"); break;
			case PLUSPLUS:  printf("PLUSPLUS\t\n"); break;
			case MINUSMINUS:  printf("MINUSMINUS\t\n"); break;
			case SHL :  printf("SHL\t\n"); break;
			case SHR :  printf("SHR\t\n"); break;
			case LTEQ :  printf("LTEQ\t\n"); break;
			case GTEQ :  printf("GTEQ\t\n"); break;
			case EQEQ :  printf("EQEQ\t\n"); break;
			case NOTEQ :  printf("NOTEQ\t\n"); break;
			case LOGAND :  printf("LOGAND\t\n"); break;
			case LOGOR :  printf("LOGOR\t\n"); break;
			case ELLIPSIS :  printf("ELLIPSIS\t\n"); break;
			case TIMESEQ :  printf("TIMESEQ\t\n"); break;
			case DIVEQ :  printf("DIVEQ\t\n"); break;
			case MODEQ :  printf("MODEQ\t\n"); break;
			case PLUSEQ :  printf("PLUSEQ\t\n"); break;
			case MINUSEQ :  printf("MINUSEQ\t\n"); break;
			case SHLEQ :  printf("SHLEQ\t\n"); break;
			case SHREQ :  printf("SHREQ\t\n"); break;
			case ANDEQ :  printf("ANDEQ\t\n"); break;
			case OREQ :  printf("OREQ\t\n"); break;
			case XOREQ :  printf("XOREQ\t\n"); break;

			case AUTO : printf("AUTO\t\n"); break;
			case BREAK : printf("BREAK\t\n"); break;
			case CASE : printf("CASE\t\n"); break;
			case CHAR : printf("CHAR\t\n"); break;
			case CONST : printf("CONST\t\n"); break;
			case CONTINUE : printf("CONTINUE\t\n"); break;
			case DEFAULT : printf("DEFAULT\t\n"); break;
			case DO : printf("DO\t\n"); break;
			case DOUBLE : printf("DOUBLE\t\n"); break;
			case ELSE : printf("ELSE\t\n"); break;
			case ENUM : printf("ENUM\t\n"); break;
			case EXTERN : printf("EXTERN\t\n"); break;
			case FLOAT : printf("FLOAT\t\n"); break;
			case FOR : printf("FOR\t\n"); break;
			case GOTO : printf("GOTO\t\n"); break;
			case IF : printf("IF\t\n"); break;
			case INLINE : printf("INLINE\t\n"); break;
			case INT : printf("INT\t\n"); break;
			case LONG : printf("LONG\t\n"); break;
			case REGISTER : printf("REGISTER\t\n"); break;
			case RESTRICT : printf("RESTRICT\t\n"); break;
			case RETURN : printf("RETURN\t\n"); break;
			case SHORT : printf("SHORT\t\n"); break;
			case SIGNED : printf("SIGNED\t\n"); break;
			case SIZEOF : printf("SIZEOF\t\n"); break;
			case STATIC : printf("STATIC\t\n"); break;
			case STRUCT : printf("STRUCT\t\n"); break;
			case SWITCH : printf("SWITCH\t\n"); break;
			case TYPEDEF : printf("TYPEDEF\t\n"); break;
			case UNION : printf("UNION\t\n"); break;
			case UNSIGNED : printf("UNSIGNED\t\n"); break;
			case VOID : printf("VOID\t\n"); break;
			case VOLATILE : printf("VOLATILE\t\n"); break;
			case WHILE : printf("WHILE\t\n"); break;
			case _BOOL : printf("_BOOL\t\n"); break;
			case _COMPLEX : printf("_COMPLEX\t\n"); break;
			case _IMAGINARY : printf("_IMAGINARY\t\n"); break;
			default: printf("%c\t\n", t);
		}
	}
}
