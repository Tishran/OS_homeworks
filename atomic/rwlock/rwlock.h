#pragma once

#include "spinlock.h"
#include "atomic.h"

#include <stdbool.h>

const int READ_LOCK = 0;
const int WRITE_LOCK = 1;
const int NO_LOCK = 2;

struct RwLock {
    bool state;
    int readers;
    struct SpinLock spinlock;
    struct SpinLock lock_spinlock;
};

inline void RwLock_Init(struct RwLock *lock) {
    lock->state = NO_LOCK;
    lock->readers = 0;
    SpinLock_Init(&lock->spinlock);
    SpinLock_Init(&lock->lock_spinlock);
}

inline void RwLock_ReadLock(struct RwLock *lock) {
    SpinLock_Lock(&lock->lock_spinlock);

    while (lock->state != WRITE_LOCK) {
        if (lock->state == NO_LOCK) {
            SpinLock_Lock(&lock->spinlock);
            lock->state = READ_LOCK;
        } else if (lock->readers > 0) {
            ++lock->readers;
        }
    }

    SpinLock_Unlock(&lock->lock_spinlock);
}

inline void RwLock_ReadUnlock(struct RwLock *lock) {
    SpinLock_Lock(&lock->lock_spinlock);

    if (lock->readers > 0) {
        --lock->readers;
    } else if (lock->readers == 0) {
        SpinLock_Unlock(&lock->spinlock);
        lock->state = NO_LOCK;
    }

    SpinLock_Unlock(&lock->lock_spinlock);
}

inline void RwLock_WriteLock(struct RwLock *lock) {
//    SpinLock_Lock(&lock->lock_spinlock);
//
//    while (lock->state != NO_LOCK) {
//        SpinLock_Lock(&lock->spinlock);
//        lock->state = WRITE_LOCK;
//    }
//
//    SpinLock_Unlock(&lock->lock_spinlock);
}

inline void RwLock_WriteUnlock(struct RwLock *lock) {
    SpinLock_Lock(&lock->lock_spinlock);

    SpinLock_Unlock(&lock->spinlock);
    lock->state = NO_LOCK;

    SpinLock_Unlock(&lock->lock_spinlock);
}
