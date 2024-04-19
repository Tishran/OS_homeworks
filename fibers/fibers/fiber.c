#include <sys/user.h>
#include <malloc.h>
#include "fiber.h"

struct Context *curr = NULL;
struct Context *tail = NULL;

void Handler(void (*func)(void *), void *data) {
    func(data);
    curr->toDel = true;

    // check here - might be wrong
    struct Context *old = curr;
    curr = curr->next;
    asm volatile ("push %0\n\t"
                  "push %1"
            :
            : "rm" (func), "rm" (data)); // will it work?
    SwitchContext(old, curr);
}

// damn, that is some shit code here
void FiberSpawn(void (*func)(void *), void *data) {
    if (curr == NULL) {
        curr = malloc(sizeof(struct Context));
        curr->toDel = false;

        curr->rip = (void *) Handler;
        curr->rsp = (void *) malloc(PAGE_SIZE);
        curr->rsp += PAGE_SIZE;
        curr->rdi = func; // idk, might be wrong, check calling conventions
        curr->rsi = data;

        tail = curr;// idk
        tail->prev = curr;
        tail->next = curr;
        curr->prev = tail;
        curr->next = tail;
    } else {
        struct Context *newContext = NULL;
        newContext = malloc(sizeof(struct Context));

        newContext->toDel = false;
        newContext->rip = (void *) Handler;
        newContext->rsp = (void *) malloc(PAGE_SIZE);
        newContext->rsp += PAGE_SIZE; // +- 1 ?
        newContext->rdi = func; // idk, might be wrong, check calling conventions
        newContext->rsi = data; // у меня в конспектах написано что так нельзя делать,
        // т к эти регистры затерутся при вызове

        // check this part later
        newContext->next = tail->next;

        tail->next = newContext;
        newContext->prev = tail;
        tail = newContext;
    }
}

void FiberYield() {
    // wtf, is this that easy???
    struct Context *old = curr;
    curr = curr->next;

    asm volatile ("push %0\n\t"
                  "push %1"
            :
            : "rm" (old->rdi), "rm" (old->rsi)); // will it work?
    SwitchContext(old, curr);

    if (old->toDel) {
        free(old->rsp);
        old->prev->next = old->next;
        old->next->prev = old->prev;

        free(curr);
    }
}

int FiberTryJoin() {
    if (curr->next == NULL && curr->prev == NULL) {
        return 1;
    }

    return 0;
}


//$rsi   : 0x8b000042d8058d48
//$rdi   : 0x00005555555562f9  →  <FuncB+0> endbr64