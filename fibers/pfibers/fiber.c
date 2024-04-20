#include "fiber.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/user.h>

#define FIBER_STACK_SIZE (4 << 12)

// Signal frame on stack.
//struct sigframe {
//    size_t r8, r9, r10, r11, r12, r13, r14, r15;
//    size_t rdi, rsi, rbp, rbx, rdx, rax, rcx, rsp;
//    size_t rip, flags;
//};

//typedef struct Fiber {
//    struct sigframe *context;
//    struct Fiber *next;
//    struct Fiber *prev;
//    void* stackPointer;
//
//    void (*func)(void *);
//
//    void *data;
//    bool toDel;
//} Fiber;

typedef struct Context {
    void *rbx, *rbp, *r12, *r13, *r14, *r15, *rsp, *rip;

    void (*func)(void *);

    void *data;
} Context;

typedef struct Fiber {
    struct Context *context;
    struct Fiber *next;
    struct Fiber *prev;
    void *stackPointer;
    bool toDel;
} Fiber;

void fiber_sched(int signum);

void FiberInit() {
//    // Need to disable stdlib buffer to use printf from fibers
    setbuf(stdout, NULL);
//
//    // Initialize timer
    signal(SIGALRM, fiber_sched);
    ualarm(1000, 1000);
}

struct Fiber *curr = NULL;

extern void SwitchContext(struct Context *current, struct Context *next);

void Handler(void (*func)(void *), void *data) {
    func(data);
    curr->toDel = true;
    FiberYield();
}

// damn, that is some shit code here
void FiberSpawn(void (*func)(void *), void *data) {
    if (curr == NULL) {
        curr = malloc(sizeof(struct Fiber));
        curr->context = malloc(sizeof(struct Context));
        curr->toDel = false;

        curr->prev = malloc(sizeof(struct Fiber));
        curr->prev->context = malloc(sizeof(struct Context));

        curr->prev->toDel = false;

        curr->prev->context->rip = Handler;
        curr->prev->stackPointer = (void *) malloc(FIBER_STACK_SIZE);
        curr->prev->context->rbp = curr->prev->stackPointer + FIBER_STACK_SIZE;
        curr->prev->context->rsp = curr->prev->stackPointer + FIBER_STACK_SIZE - 8;

        curr->prev->context->func = func;
        curr->prev->context->data = data;

        curr->prev->prev = curr;
        curr->prev->next = curr;
        curr->next = curr->prev;
    } else {
        struct Fiber *newFiber = malloc(sizeof(struct Fiber));
        newFiber->context = malloc(sizeof(struct Context));

        newFiber->toDel = false;
        newFiber->context->rip = Handler;
        newFiber->stackPointer = (void *) malloc(FIBER_STACK_SIZE);
        newFiber->context->rbp = newFiber->stackPointer + FIBER_STACK_SIZE;
        newFiber->context->rsp = newFiber->stackPointer + FIBER_STACK_SIZE - 8;

        newFiber->context->func = func;
        newFiber->context->data = data;

        newFiber->next = curr;
        newFiber->prev = curr->prev;
        curr->prev->next = newFiber;
        curr->prev = newFiber;
    }
}

void FiberYield() {
    // wtf, is this that easy???
    struct Fiber *old = curr;
    curr = old->next;

    while (curr->toDel) {
        curr = curr->next;
    }

    asm volatile ("push %0\n\t"
                  "push %1"
            :
            : "rm" (old->context->func), "rm" (old->context->data));
    SwitchContext(old->context, curr->context);
}

int FiberTryJoin() {
    int cnt = 1;
    struct Fiber *fiber = curr->next;
    while (fiber != curr) {
        if (!fiber->toDel) ++cnt;
        fiber = fiber->next;
    }



    if (cnt == 1) {
        return 1;
    }

    return 0;
}

void fiber_sched(int signum) {
    FiberYield();
}