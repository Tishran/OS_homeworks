#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

extern size_t PAGE_SIZE;
extern double *SQRTS;

void CalculateSqrts(double *sqrt_pos, int start, int n);

void HandleSigsegv(int sig, siginfo_t *siginfo, void *ctx) {
    exit(0);
//    void *fault_address = siginfo->si_addr;
//    size_t offset = (size_t) (fault_address - (void *) SQRTS);
//
//    int page_number = offset / PAGE_SIZE;
//    void* addr = (void*) SQRTS + page_number * PAGE_SIZE;
//    void *page_start = mmap(addr, PAGE_SIZE,
//                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
//
//    if (page_start == MAP_FAILED) {
//        if (errno == ENOMEM) {
//            munmap((void*) SQRTS, page_number * PAGE_SIZE);
//            page_start = mmap(addr, PAGE_SIZE,
//                                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
//        } else {
//            perror("mmap error");
//            exit(EXIT_FAILURE);
//        }
//    }
//
//    CalculateSqrts(page_start, page_number * PAGE_SIZE / sizeof(double), PAGE_SIZE / sizeof(double));
}
