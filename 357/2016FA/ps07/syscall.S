.data
    hw: .ascii "Hello world\n"

.text
.global _start

_start:
    movq $1, %rax #syscall number for write(2) according to /usr/include/asm/unistd_64.h
    movq $1, %rdi #stdout fd
    movq $hw, %rsi #address of defined string
    movq $12, %rdx #length of defined string
    syscall
    
    movq $60, %rax #exit(2) syscall number
    movq $3, %rdi #status code
    syscall
