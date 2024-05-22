#include "fiber.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/user.h>
#include <ucontext.h>

#define FIBER_STACK_SIZE (4 << 12)

struct Fiber {
    ucontext_t ucontext;
    struct Fiber *next;
};

struct Fiber *currFib = NULL;
struct Fiber *mainFib = NULL;
ucontext_t ctx;

void FillFiberStruct(struct Fiber** fiber) {
    (*fiber)->ucontext.uc_stack.ss_sp = malloc(FIBER_STACK_SIZE);
    (*fiber)->ucontext.uc_stack.ss_size = FIBER_STACK_SIZE;
    (*fiber)->ucontext.uc_link = NULL;
    (*fiber)->next = mainFib ? mainFib->next : NULL;
    if (mainFib) {
        mainFib->next = *fiber;
    } else {
        mainFib = *fiber;
    }
}

void FiberSpawn(void (*func)(void *), void *data) {
    struct Fiber *fiber = (struct Fiber *) malloc(sizeof(struct Fiber));
    getcontext(&fiber->ucontext);
    FillFiberStruct(&fiber);
    getcontext(&ctx);
    makecontext(&fiber->ucontext, (void (*)(void)) func, 1, data);
    swapcontext(&ctx, &fiber->ucontext);
}

void FiberYield() {
    if (currFib) {
        currFib = currFib->next;
        if (!currFib) {
            currFib = mainFib;
        }
    } else {
        currFib = mainFib;
    }

    if (currFib) {
        swapcontext(&mainFib->ucontext, &currFib->ucontext);
    }
}

int FiberTryJoin() {
    return (!mainFib || !mainFib->next);
}

void fiber_sched(int signum) {
    if (signum == SIGALRM) {
        FiberYield();
    }
};

void FiberInit() {
    setbuf(stdout, NULL);
    signal(SIGALRM, fiber_sched);
    ualarm(1000, 1000);
}