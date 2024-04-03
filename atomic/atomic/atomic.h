#pragma once


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

inline void AtomicAdd(int64_t *atomic, int64_t value) {
    asm volatile("lock\n\t"
                 "add %1, %0\n\t"
            : "+m" (*atomic)
            : "r" (value)
            : "memory");
}

inline void AtomicSub(int64_t *atomic, int64_t value) {
    asm volatile("lock\n\t"
                 "sub %1, %0\n\t"
            : "+m" (*atomic)
            : "r" (value)
            : "memory");
}

inline int64_t AtomicXchg(int64_t *atomic, int64_t value) {
    asm volatile("xchg %1, %0\n\t"
            : "+m" (*atomic), "+r" (value)
            :
            : "memory");

    return value;
}

inline int64_t AtomicCas(int64_t *atomic, int64_t *expected, int64_t value) {
    bool result = 0;
    asm volatile("lock\n\t"
                 "cmpxchgq %3, %1\n\t"
                 "setz %0"
            : "=r" (result), "+m" (*atomic), "+a" (*expected)
            : "rm" (value)
            : "memory");

    return result;
}
