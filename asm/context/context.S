        .global savectx
        .global jumpctx

savectx:
    mov (%rsp), %rax
    mov %rax, (%rdi)

    mov %rsp, 8(%rdi)
    add $8, 8(%rdi)
    mov %rbp, 16(%rdi)

    mov $0, %rax
    ret

jumpctx:
    mov 8(%rdi), %rsp
    push (%rdi)
    mov 16(%rdi), %rbp

    mov $1, %rax
    ret
