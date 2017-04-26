all: lexer

# Lexer
lexer: lex.yy.c
	@echo Building lexer...
	@gcc lex.yy.c -I includes -std=gnu99 -lfl -o $@

lex.yy.c: src/lexer.l
	@echo Generating lexer...
	@flex -l $<

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
	@rm -rf lexer lex.yy.c
	@echo Removing lexer test files...
	@rm -rf ltests/ltest.my{out,err}
