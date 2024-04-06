#include "fiber.h"

#include <signal.h>
#include <stdlib.h>

#define FIBER_STACK_SIZE (4 << 12)

// Signal frame on stack.
struct sigframe {
    size_t r8, r9, r10, r11, r12, r13, r14, r15;
    size_t rdi, rsi, rbp, rbx, rdx, rax, rcx, rsp;
    size_t rip, flags;
};

void fiber_sched(int signum);

void FiberInit() {
    // Need to disable stdlib buffer to use printf from fibers
    setbuf(stdout, NULL);

    // Initialize timer
    signal(SIGALRM, fiber_sched);
    ualarm(1000, 1000);
}

// Your code here
