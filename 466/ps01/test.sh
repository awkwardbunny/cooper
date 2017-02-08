#!/bin/sh
flex lan.l
gcc lex.yy.c -lfl -o lan.out
gcc -E ltests/*.c | ./lan.out > output 2> error
diff output ltests/ltest.out
diff error ltests/ltest.err
