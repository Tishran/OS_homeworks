#include <kern/sysgate.h>
#include <kern/syscall.h>

// Your code here

inline long read_msr(int msr) {
    long value;
    asm volatile ( "rdmsr" : "=A" (value) : "c" (msr) );

    return value;
}

inline void write_msr(int msr, long value) {
//    int low = value & 0xFFFFFFFF;
//    int high = value >> 32;
//    asm volatile (
//            "wrmsr"
//            :
//            : "c"(msr), "a"(low), "d"(high)
//            );
    asm volatile ( "wrmsr" : : "c" (msr), "A" (value) );
}

void _syscall_enter();

void sysgate() {
    write_msr(IA32_EFER, read_msr(IA32_EFER) + 1); // это точно выставит нулевой бит? кажется да
    write_msr(IA32_LSTAR, (long) _syscall_enter);

    asm volatile("sysretq");
}
