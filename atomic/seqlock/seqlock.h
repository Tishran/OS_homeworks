#pragma once

#include <stdint.h>

struct SeqLock {
};

inline void SeqLock_Init(struct SeqLock* lock) {
}

inline int64_t SeqLock_ReadLock(struct SeqLock* lock) {
}

inline int SeqLock_ReadUnlock(struct SeqLock* lock, int64_t value) {
}

inline void SeqLock_WriteLock(struct SeqLock* lock) {
}

inline void SeqLock_WriteUnlock(struct SeqLock* lock) {
}
