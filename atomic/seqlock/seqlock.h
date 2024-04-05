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
    return lock->state;
}

inline int SeqLock_ReadUnlock(struct SeqLock* lock, int64_t value) {
    return AtomicCas(&lock->state, &value, 0);
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