    .globl _start
_start:
    movq (%rsp), %rdi
    leaq 8(%rsp), %rsi
    leaq 16(%rsp, %rdi, 8), %rdx
    call main
    movq %rax, %rdi
    movl $60, %eax
    syscall
