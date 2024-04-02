#pragma once

struct SpinLock {
    int curr_state;
};

inline void SpinLock_Init(struct SpinLock *lock) {
    lock->curr_state = 0;
}

inline void SpinLock_Lock(struct SpinLock *lock) {
    int unlocked = 0;
    int locked = 1;

    asm volatile("while_lock:\n\t"
                 "movl $0, %%eax\n\t"
                 "lock\n\t"
                 "cmpxchg %2, %0\n\t"
                 "jne while_lock"
            : "+m" (lock->curr_state), "+a" (unlocked)
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
