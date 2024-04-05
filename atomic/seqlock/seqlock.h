#pragma once

#include "atomic.h"
#include "spinlock.h"

#include <stdint.h>

struct SeqLock {
    int64_t state;
    struct SpinLock lock_state;
};

inline void SeqLock_Init(struct SeqLock* lock) {
    lock->state = 0;
    SpinLock_Init(&lock->lock_state);
}

inline int64_t SeqLock_ReadLock(struct SeqLock* lock) {
    SpinLock_Lock(&lock->lock_state);
    int64_t res = lock->state;
    SpinLock_Unlock(&lock->lock_state);

    return res;
}

inline int SeqLock_ReadUnlock(struct SeqLock* lock, int64_t value) {
    int res = 0;
    SpinLock_Lock(&lock->lock_state);
    if (value == lock->state % 2 && lock->state % 2 == 0) {
        res = 1;
    }

    SpinLock_Unlock(&lock->lock_state);

    return res;
}

inline void SeqLock_WriteLock(struct SeqLock* lock) {
    SpinLock_Lock(&lock->lock_state);
    ++lock->state;
    SpinLock_Unlock(&lock->lock_state);
}

inline void SeqLock_WriteUnlock(struct SeqLock* lock) {
    SpinLock_Unlock(&lock->lock_state);
    ++lock->state;
    SpinLock_Unlock(&lock->lock_state);
}
