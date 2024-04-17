#include <kern/sysgate.h>
#include <kern/syscall.h>

extern void _start_user();
void _syscall_enter();

inline long read_msr(int msr) {
    long value;
    asm volatile ( "rdmsr" : "=A" (value) : "c" (msr));

    return value;
}

inline void write_msr(int msr, long value) {
    int low = value & 0xFFFFFFFF;
    int high = value >> 32;
    asm volatile (
            "wrmsr"
            :
            : "c"(msr), "a"(low), "d"(high)
            );
}

void sysgate() {
    write_msr(IA32_EFER, read_msr(IA32_EFER) + 1);
    write_msr(IA32_LSTAR, (long) &_syscall_enter);

    asm volatile("mov %0, %%rcx\n\t"
                 "sysretq"
            :
            : "r" (&_start_user)
            : "rcx");
}
