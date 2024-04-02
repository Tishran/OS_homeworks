#pragma once

struct SpinLock {
    int curr_state;
};

inline void SpinLock_Init(struct SpinLock *lock) {
    lock->curr_state = 0;
}

inline void SpinLock_Lock(struct SpinLock *lock) {
    int locked = 1;

    asm volatile("1:\n\t"
                 "movl $0, %%eax\n\t"
                 "lock\n\t"
                 "cmpxchg %1, %0\n\t"
                 "jne 2f\n\t"
                 "jmp 3f\n\t"
                 "2:\n\t"
                 "pause\n\t"
                 "jmp 1b\n\t"
                 "3:\n\t"
            : "+m" (lock->curr_state)
            : "r" (locked)
            : "memory");

    __sync_synchronize();
}

inline void SpinLock_Unlock(struct SpinLock *lock) {
    __sync_synchronize();
    int unlocked = 0;
    asm volatile("xchg %0, %1"
            : "+r" (lock->curr_state), "+r" (unlocked)
            :
            : "memory");
}
