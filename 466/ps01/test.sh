#!/bin/sh
flex lan.l
gcc lex.yy.c -lfl -o lan.out
gcc -E ltests/*.c | ./lan.out > output
diff output ltests/ltest.out
