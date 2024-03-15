    .text
    .globl exit
    .globl write
    .globl mmap
    .globl munmap
    .globl mremap
    .globl msync
exit:
    movl $60, %eax
    jmp scall
write:
    movl $1, %eax
    jmp scall
mmap:
    movl $9, %eax
    jmp scall
munmap:
    movl $11, %eax
    jmp scall
mremap:
    movl $25, %eax
    jmp scall
msync:
    movl $26, %eax
    jmp scall
scall:
    movq %rcx, %r10
    syscall
    retq
