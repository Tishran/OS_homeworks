#pragma once

#include "atomic.h"
#include "spinlock.h"

#include <stdint.h>
#include <threads.h>
#include <unistd.h>

struct SeqLock {
    volatile int64_t state;
    struct SpinLock lock;
};

inline void SeqLock_Init(struct SeqLock* lock) {
    lock->state = 0;
    SpinLock_Init(&lock->lock);
}

inline int64_t SeqLock_ReadLock(struct SeqLock* lock) {
    return lock->state;
}

inline int SeqLock_ReadUnlock(struct SeqLock* lock, int64_t value) {
    while (lock->state % 2 == 1) {
        thrd_yield();
    }

    return value == lock->state;
}

inline void SeqLock_WriteLock(struct SeqLock* lock) {
    SpinLock_Lock(&lock->lock);
    AtomicAdd(&lock->state, 1);
}

inline void SeqLock_WriteUnlock(struct SeqLock* lock) {
    AtomicAdd(&lock->state, 1);
    SpinLock_Unlock(&lock->lock);
}