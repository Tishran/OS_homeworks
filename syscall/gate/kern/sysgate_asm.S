.global _syscall_enter

_syscall_enter:
    push %rcx
    cmp $1, %edi
    je call_sys_work
    jmp call_sys_retire

    call_sys_work:
        mov %rsi, %rdi
        call sys_work
        jmp end

    call_sys_retire:
        call sys_retire
        jmp end

    end:
    mov $5, %rax
    pop %rcx
    sysretq
