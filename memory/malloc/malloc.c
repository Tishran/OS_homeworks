#include "malloc.h"

void *malloc(size_t size) {
    size += sizeof(size_t);
    void *ptr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL;
    }

    *((size_t *) ptr) = size;

    return (void *) (ptr + sizeof(size_t));
}

void *realloc(void *ptr, size_t size) {
    ptr -= sizeof(size_t);
    size_t old_size = *((size_t *) ptr);
    size_t new_size = size + sizeof(size_t);
    void *new_ptr = mremap(ptr, old_size, new_size, MREMAP_MAYMOVE, NULL);
    if (new_ptr == MAP_FAILED) {
        return NULL;
    }

    *((size_t *) new_ptr) = new_size;

    return (void *) (new_ptr + sizeof(size_t));
}

void free(void *ptr) {
    ptr -= sizeof(size_t);
    munmap(ptr, *((size_t *) ptr));
}