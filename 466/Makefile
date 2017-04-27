all: parser

CFLAGS=-I includes -std=gnu99 -lfl

# Parser
parser: src/parser.tab.c src/lex.yy.c
	@echo Building lexer + parser...
	@gcc -o $@ $^ $(CFLAGS)

src/parser.tab.c includes/parser.tab.h: src/parser.y
	@echo Generating parser...
	@bison -d $< -o src/parser.tab.c --defines=includes/parser.tab.h

# Lexer
lexer: src/lex.yy.c
	@echo Building lexer...
	@gcc -o $@ $^ $(CFLAGS)

src/lex.yy.c: src/lexer.l includes/parser.tab.h
	@echo Generating lexer...
	@flex -l -o $@ $<

# Lexer Tests
LTESTS = $(wildcard ltests/*.c)

.PHONY: ltests
ltests ltests/ltest.myout ltests/ltest.myerr: lexer
	@echo Running tests through lexer...
	@gcc -E ltests/*.c | ./$< > ltests/ltest.myout 2> ltests/ltest.myerr
	@echo Comparing output
	@diff ltests/ltest.{out,myout} | less
	@diff ltests/ltest.{err,myerr} | less

.PHONY: clean
clean:
	@echo Removing lexer files...
	@rm -rf lexer src/lex.yy.c
	@echo Removing lexer test files...
	@rm -rf ltests/ltest.my{out,err}
	@echo Removing parser files
	@rm -rf parse src/parser.tab.c includes/parser.tab.h
