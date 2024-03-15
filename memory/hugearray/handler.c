#include <stdlib.h>

extern size_t PAGE_SIZE;
extern double* SQRTS;

void CalculateSqrts(double* sqrt_pos, int start, int n);

void HandleSigsegv(int sig, siginfo_t* siginfo, void* ctx) {
    // Your code here
}
