        .global savectx
        .global jumpctx

savectx:
    pop %rax
    mov %rax, (%rdi)
    push %rax

    mov %rsp, 8(%rdi)
    mov %rbp, 16(%rdi)
    ret

jumpctx:
    pop %rax
    mov (%rdi), %rax
    push %rax

    mov 8(%rdi), %rsp
    mov 16(%rdi), %rbp
    mov $1, %rax
    ret
