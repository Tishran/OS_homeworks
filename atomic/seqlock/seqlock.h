#pragma once

#include "atomic.h"
#include "spinlock.h"

#include <stdint.h>
#include <unistd.h>

struct SeqLock {
    int64_t state;
    struct SpinLock lock;
};

inline void SeqLock_Init(struct SeqLock* lock) {
    lock->state = 0;
    SpinLock_Init(&lock->lock);
}

inline int64_t SeqLock_ReadLock(struct SeqLock* lock) {
    SpinLock_Lock(&lock->lock);
    int64_t res = lock->state;
    SpinLock_Unlock(&lock->lock);
    return res;
}

inline int SeqLock_ReadUnlock(struct SeqLock* lock, int64_t value) {
    SpinLock_Lock(&lock->lock);

    int res = (value & 1) | (lock->state ^ value);

    SpinLock_Unlock(&lock->lock);

    return res;
}

inline void SeqLock_WriteLock(struct SeqLock* lock) {
    SpinLock_Lock(&lock->lock);
//    fprintf(stderr, "%s\n", "writelock");
    ++lock->state;
}

inline void SeqLock_WriteUnlock(struct SeqLock* lock) {
//    fprintf(stderr, "%s\n", "writeUnlock");
    ++lock->state;
    SpinLock_Unlock(&lock->lock);
}