#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>

size_t PAGE_SIZE;
double* SQRTS;

void HandleSigsegv(int sig, siginfo_t* siginfo, void* ctx);

const int MEM_LIMIT = 1 << 25;
const int MAX_SQRTS = 1 << 27;

void CalculateSqrts(double* sqrt_pos, int start, int n) {
    for (int i = 0; i < n; i++) {
        sqrt_pos[i] = sqrt((double)(start + i));
    }
}

static void Setup() {
    SQRTS = mmap(NULL, MAX_SQRTS * sizeof(double) + MEM_LIMIT, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (SQRTS == MAP_FAILED) {
        fprintf(stderr, "Couldn't mmap() region for sqrt table: %s\n", strerror(errno));
        exit(1);
    }

    if (munmap(SQRTS, MAX_SQRTS * sizeof(double) + MEM_LIMIT) == -1) {
        fprintf(stderr, "Couldn't munmap() region for sqrt table: %s\n", strerror(errno));
        exit(1);
    }

    struct rlimit lim = {MEM_LIMIT, MEM_LIMIT};
    if (setrlimit(RLIMIT_AS, &lim) == -1) {
        fprintf(stderr, "Couldn't set rlimit on RLIMIT_AS: %s\n", strerror(errno));
        exit(1);
    }

    struct sigaction act;
    act.sa_sigaction = HandleSigsegv;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGSEGV, &act, NULL) == -1) {
        fprintf(stderr, "Couldn't set up SIGSEGV handler: %s\n", strerror(errno));
        exit(1);
    }
}

static void Test() {
    int pos;
    double correct_sqrt;

    srand(0xDCBA1234);

    for (int i = 0; i < 500000; ++i) {
        if (i % 2 == 0) {
            pos = rand() % (MAX_SQRTS - 1);
        } else {
            pos += 1;
        }

        CalculateSqrts(&correct_sqrt, pos, 1);
        if (SQRTS[pos] != correct_sqrt) {
            fprintf(stderr, "Square root is incorrect. Expected %f, got %f.\n", correct_sqrt, SQRTS[pos]);
            exit(1);
        }
    }
}

int main() {
    PAGE_SIZE = sysconf(_SC_PAGESIZE);

    Setup();
    Test();

    return 0;
}
