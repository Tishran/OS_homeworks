#pragma once

#include "spinlock.h"
#include "atomic.h"

const int64_t UNLOCK = 0;
const int64_t RLOCK = 1;
const int64_t WLOCK = -1;

struct RwLock {
    int64_t state;
};

inline void RwLock_Init(struct RwLock *lock) {
    lock->state = UNLOCK;
}

inline void RwLock_ReadLock(struct RwLock *lock) {
    int64_t expected_unlock = UNLOCK;
    int64_t expected_rlock = RLOCK;
    while (AtomicCas(&lock->state, &expected_unlock, RLOCK) != 1 ||
           AtomicCas(&lock->state, &expected_rlock, RLOCK) != 1) {
        asm volatile("pause");
    }

    AtomicAdd(&lock->state, 1);
}

inline void RwLock_ReadUnlock(struct RwLock *lock) {
    int64_t expected_readers_cnt = 2;
    AtomicCas(&lock->state, &expected_readers_cnt, RLOCK);
    AtomicSub(&lock->state, 1);
}

inline void RwLock_WriteLock(struct RwLock *lock) {
    int64_t expected_unlock = UNLOCK;
    while (AtomicCas(&lock->state, &expected_unlock, WLOCK) != 1) {
        asm volatile("pause");
    }
}

inline void RwLock_WriteUnlock(struct RwLock *lock) {
    AtomicXchg(&lock->state, UNLOCK);
}