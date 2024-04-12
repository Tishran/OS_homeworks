.global _syscall_enter
.global sys_work
.global sys_retire

_syscall_enter:
    push %rcx
    cmp $1, %edi
    je call_sys_work
    jmp call_sys_retire

    call_sys_work:
        call sys_work
        jmp end

    call_sys_retire:
        call sys_retire
        jmp end

    end:
    pop %rcx
    sysretq
