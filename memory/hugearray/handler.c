#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

extern size_t PAGE_SIZE;
extern double *SQRTS;
extern int MAX_SQRTS;

void CalculateSqrts(double *sqrt_pos, int start, int n);

//debug variable
int idx = 0;

void HandleSigsegv(int sig, siginfo_t *siginfo, void *ctx) {
//    fprintf(stderr, "%s: %zu\n", "handle sigsev func", idx * PAGE_SIZE);
    ++idx;
    void *fault_address = siginfo->si_addr;
    size_t offset = (size_t) (fault_address - (void *) SQRTS);

    size_t page_number = offset / PAGE_SIZE;
    void *page_start = mmap((void *) SQRTS + PAGE_SIZE * page_number, PAGE_SIZE,
                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

    if (page_start == MAP_FAILED) {
        if (errno == ENOMEM) {
//            fprintf(stderr, "%s\n", "NO MEM HAPPENED!!!!!!!!!!!!!!!!!!!!!!!!!!");
            munmap((void*) SQRTS, PAGE_SIZE * page_number);
//            SQRTS = (double *) ((void *) SQRTS + page_number * PAGE_SIZE);
            page_start = mmap((void *) SQRTS + PAGE_SIZE * page_number, PAGE_SIZE,
                                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
//            fprintf(stderr, "%d %d\n", page_start == MAP_FAILED, errno == ENOMEM);
        } else {
            perror("mmap error");
            exit(EXIT_FAILURE);
        }
    }

    int start_value = page_number * PAGE_SIZE;
//    fprintf(stderr, "%d\n", 40);
    CalculateSqrts(page_start, start_value / sizeof(double), PAGE_SIZE / sizeof(double));
}
