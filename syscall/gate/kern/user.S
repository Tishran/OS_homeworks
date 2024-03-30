    .global _start_user
    .code64
    .text
_start_user:
    movw %cs, %ax
    andw $3, %ax
    cmpw $3, %ax
    jnz epicfail
    movl $1, %edi
    movabs $user_data, %rsi
    syscall
    cmpq $5, %rax
    jne epicfail
    movl $2, %edi
    syscall

epicfail:
    movl $0xFFFFFFF0, %eax
    jmp *(%eax)

    .section .rodata
user_data:
    .byte 0x13, 0x7a, 0x3b, 0x37, 0x7a, 0x2d, 0x35, 0x28, 0x31, 0x33, 0x34, 0x3d, 0x7b, 0x00
