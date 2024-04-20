#include <sys/user.h>
#include <malloc.h>
#include <stdint.h>
#include "fiber.h"

struct Fiber *curr = NULL;
//struct Fiber *tail = NULL;

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

        curr->prev->context->rip = (void *) Handler;
        curr->prev->context->start = (void *) malloc(PAGE_SIZE);
        curr->prev->context->rbp = curr->prev->context->start + PAGE_SIZE;
        curr->prev->context->rsp = curr->prev->context->start + PAGE_SIZE - 8;

//        *((char*) tail->context->rsp) = (char*) data;

        curr->prev->context->func = func;
        curr->prev->context->data = data;

        curr->prev->prev = curr;
        curr->prev->next = curr;
        curr->next = curr->prev;
    } else {
        struct Fiber *newFiber = malloc(sizeof(struct Fiber));
        newFiber->context = malloc(sizeof(struct Context));

        newFiber->toDel = false;
        newFiber->context->rip = (void *) Handler;
        newFiber->context->start = (void *) malloc(PAGE_SIZE);
        newFiber->context->rbp = newFiber->context->start + PAGE_SIZE;
        newFiber->context->rsp = newFiber->context->start + PAGE_SIZE - 8;

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
    struct Fiber* fiber = curr->next;
    while (fiber != curr) {
        if (!fiber->toDel) ++cnt;
        fiber = fiber->next;
    }

    if (cnt == 1) {
        return 1;
    }

    return 0;
}


//$rsi   : 0x8b000042d8058d48
//$rdi   : 0x00005555555562f9  →  <FuncB+0> endbr64