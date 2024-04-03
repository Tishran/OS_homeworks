#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <asm/unistd_64.h>

#include "spinlock.h"
#include "atomic.h"

enum rwlockstate {
    RLOCK, WLOCK, UNLOCK
};

struct RwLock {
    enum rwlockstate state;
    int64_t readers;
    struct SpinLock spinlock;
};

inline void RwLock_Init(struct RwLock *lock) {
    lock->state = UNLOCK;
    lock->readers = 0;
    SpinLock_Init(&lock->spinlock);
}

inline void RwLock_ReadLock(struct RwLock *lock) {
    usleep(500000);
    SpinLock_Lock(&lock->spinlock);
    AtomicXchg(&lock->readers, lock->readers);

    lock->state = RLOCK;
    lock->readers++;

    SpinLock_Unlock(&lock->spinlock);
}

inline void RwLock_ReadUnlock(struct RwLock *lock) {
    SpinLock_Lock(&lock->spinlock);

    --lock->readers;
    if (lock->readers == 0) {
        lock->state = UNLOCK;
    }

    SpinLock_Unlock(&lock->spinlock);
}

inline void RwLock_WriteLock(struct RwLock *lock) {
    SpinLock_Lock(&lock->spinlock);

    while (lock->state != UNLOCK) {
        SpinLock_Unlock(&lock->spinlock);
        SpinLock_Lock(&lock->spinlock);
    }

    lock->state = WLOCK;
}

inline void RwLock_WriteUnlock(struct RwLock *lock) {
    lock->state = UNLOCK;
    SpinLock_Unlock(&lock->spinlock);
}