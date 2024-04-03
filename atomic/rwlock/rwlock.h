#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <asm/unistd_64.h>

#include "spinlock.h"
#include "atomic.h"

//enum rwlockstate {
//    RLOCK, WLOCK, UNLOCK
//};
//int64_t UNLOCK = 0;
//int64_t RLOCK = 1;
//int64_t WLOCK = -1;

struct RwLock {
    int64_t writing;
    int64_t readers;
};

inline void RwLock_Init(struct RwLock *lock) {
    lock->writing = 0; // -1 - writing is permitted
    lock->readers = 0;
}

inline void RwLock_ReadLock(struct RwLock *lock) {
    int64_t expected_writing = 0;
    while (AtomicCas(&lock->writing, &expected_writing, -1) != 1) {
        asm volatile("pause");
    }

    AtomicAdd(&lock->readers, 1);
}

inline void RwLock_ReadUnlock(struct RwLock *lock) {
    AtomicAdd(&lock->writing, 1);
    AtomicSub(&lock->readers, 1);
}

inline void RwLock_WriteLock(struct RwLock *lock) {
    int64_t expected_reading = 0;
    int64_t expected_writing = 0;
    while (AtomicCas(&lock->readers, &expected_reading, 0) != 1 &&
           AtomicCas(&lock->writing, &expected_writing, 1) != 1) {
        asm volatile("pause");
    }
}

inline void RwLock_WriteUnlock(struct RwLock *lock) {
    AtomicSub(&lock->writing, 1);
}