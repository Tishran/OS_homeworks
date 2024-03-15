#pragma once

typedef long ssize_t;
typedef unsigned long size_t;
typedef unsigned long off_t;
typedef unsigned long intptr_t;

#define NULL           ((void*)  0)
#define MAP_FAILED     ((void*) -1)

#define PROT_READ       0x1
#define PROT_WRITE      0x2

#define MAP_PRIVATE     0x02
#define MAP_ANONYMOUS   0x20
#define MAP_FIXED       0x10

#define MREMAP_MAYMOVE  1

#define MS_ASYNC        1

#define ENOMEM          12

void exit(int status);
ssize_t write(int fd, const void* buf, size_t count);

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void* addr, size_t length);
void* mremap(void* old_addr, size_t old_size, size_t new_size, int flags, void* new_addr);
int msync(void* addr, size_t length, int flags);
