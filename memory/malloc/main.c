#include "syscall.h"
#include "malloc.h"

void assert(int condition) {
    if (!condition) {
        exit(1);
    }
}

void Validate(char *p, char c, size_t size) {
    for (int j = 0; j < size; ++j) {
        assert(((char*)p)[j] == c);
    }
}

void memset(void* ptr, char c, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        ((char*)ptr)[i] = c;
    }
}

size_t rdtsc() {
    unsigned int hi, lo;
    asm volatile ("rdtscp\n\tlfence" : "=a"(lo), "=d"(hi) :: "ecx");
    return (((size_t)hi) << 32) | lo;
}

void PrintNum(size_t x) {
    char buf[17] = {[0 ... 15] = '0', [16] = '\n'};
    int i = 15;
    while (x > 0) {
        size_t c = x % 16;
        buf[i] = c < 10 ? '0' + c : 'a' + c - 10;
        x >>= 4;
        --i;
    }
    write(0, buf, 17);
}

void PrintStr(const char* s) {
    size_t size = 0;
    while (s[size] != '\0') {
        ++size;
    }
    write(0, s, size);
}

void Debug(const char* s, size_t x) {
    PrintStr(s);
    PrintNum(x);
}

void TestSimple() {
    void* p = malloc(1);
    assert(p != NULL);
    *(char*)p = 'x';
    free(p);
}

void TestTwo() {
    intptr_t p1 = (intptr_t)malloc(1024);
    intptr_t p2 = (intptr_t)malloc(1024);
    assert(p1 < p2 || p1 >= p2 + 1024);
    assert(p2 < p1 || p2 >= p1 + 1024);
    memset((void*)p1, 'a', 1024);
    memset((void*)p2, 'b', 1024);
    free((void*)p1);
    free((void*)p2);
}

void TestHuge() {
    size_t size = 1 << 25;
    void* p = malloc(size);
    memset(p, '?', size);
    free(p);
}

void TestRealloc() {
    size_t size = 1;
    void* p = malloc(size);

    for (int i = 0; i < 25; ++i) {
        assert(p != NULL);
        char c = 'A' + i % 26;
        memset(p, c, size);
        p = realloc(p, size << 1);
        assert(p != NULL);
        Validate(p, c, size);
        size <<= 1;
    }

    free(p);
}

void TestReallocSpeed() {
    size_t size = 1 << 29;
    void* p = malloc(size);
    memset(p, 0, size);
    Debug("allocated=", (size_t)p);

    intptr_t lp = (intptr_t) p + size;
    lp = (lp + 4095) & ~(4095UL);
    void* l = mmap((void*)lp, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0);
    assert((intptr_t)l == lp);
    memset(l, 'x', 4096);
    Debug("guard page=", (size_t)l);

    size_t begin = rdtsc();
    memset(p, 'a', size);
    size_t end = rdtsc();
    size_t memset_time = end - begin;

    begin = rdtsc();
    p = realloc(p, size << 1);
    end = rdtsc();
    size_t realloc_time = end - begin;
    Debug("reallocated=", (size_t)p);

    Debug("memset time=", memset_time);
    Debug("realloc time=", realloc_time);

    assert(realloc_time * 2 < memset_time);

    memset(p + size, 'b', size);
    Validate(l, 'x', 4096);

    free(p);
    munmap(l, 4096);
}

void TestFree() {
    size_t size = 1 << 20;
    void* p = malloc(size);
    memset(p, 0, size);
    free(p);

    intptr_t lp = (intptr_t)p;
    lp = lp & ~(4095UL);
    for (int i = 0; i < size; i += 4096) {
        int r = msync((void*)(lp + i), 4096, MS_ASYNC);
        assert(r == -ENOMEM);
    }
}

int main() {
    TestSimple();
    TestTwo();
    TestHuge();
    TestRealloc();
    TestReallocSpeed();
    TestFree();

    return 0;
}
