#include <stddef.h>
#include <malloc.h>
#include <sys/procfs.h>
#include "fiber.h"

struct ContextList *ctxList;

void Handler(void (*func)(void *), void *data) {
    func(data);
    ctxList->curr->toDel = true;

    // check here - might be wrong
    struct Context *old = ctxList->curr;
    ctxList->curr = ctxList->curr->next;
    asm volatile ("push %0\n\t"
                  "push %1"
            :
            : "rm" (func), "rm" (data)); // will it work?
    SwitchContext(old, ctxList->curr);
}

// damn, that is some shit code here
void FiberSpawn(void (*func)(void *), void *data) {
    if (ctxList == NULL) {
        ctxList = malloc(sizeof(struct ContextList));
        ctxList->curr = NULL;
        ctxList->tail = NULL;
        ctxList->head = NULL;
    }

    if (ctxList->curr == NULL) {
        ctxList->curr = malloc(sizeof(struct Context));
        ctxList->curr->toDel = false;

        ctxList->curr->rip = (void *) Handler;
        ctxList->curr->rsp = (void *) malloc(PAGE_SIZE) + PAGE_SIZE;
        ctxList->curr->rdi = func; // idk, might be wrong, check calling conventions
        ctxList->curr->rsi = data;

        ctxList->head = ctxList->curr;
        ctxList->tail = ctxList->curr;// idk
        ctxList->head->next = ctxList->curr;
        ctxList->tail->prev = ctxList->curr;
        ctxList->head->prev = ctxList->tail;
        ctxList->tail->next = ctxList->head;

        ctxList->head->next = ctxList->head;
        ctxList->head->prev = ctxList->head;
    } else {
        struct Context *newContext = NULL;
        newContext = (struct Context *) malloc(sizeof(struct Context));
        (void) newContext;
        newContext->toDel = false;
        newContext->rip = (void *) Handler;
        newContext->rsp = (void *) malloc(PAGE_SIZE);
        newContext->rsp += PAGE_SIZE; // +- 1 ?
        newContext->rdi = func; // idk, might be wrong, check calling conventions
        newContext->rsi = data; // у меня в конспектах написано что так нельзя делать,
        // т к эти регистры затерутся при вызове

        // check this part later
        newContext->next = ctxList->head;

        ctxList->tail->next = newContext;
        newContext->prev = ctxList->tail;
        ctxList->tail = newContext;
    }
}

void FiberYield() {
    // wtf, is this that easy???
    struct Context *old = ctxList->curr;
    ctxList->curr = ctxList->curr->next;

    asm volatile ("push %0\n\t"
                  "push %1"
            :
            : "rm" (old->rdi), "rm" (old->rsi)); // will it work?
    SwitchContext(old, ctxList->curr);

    if (old->toDel) {
        free(old->rsp);
        old->prev->next = old->next;
        old->next->prev = old->prev;

        free(ctxList->curr);
    }
}

int FiberTryJoin() {
    if (ctxList->curr->next == NULL && ctxList->curr->prev == NULL) {
        return 1;
    }

    return 0;
}


//$rsi   : 0x8b000042d8058d48
//$rdi   : 0x00005555555562f9  →  <FuncB+0> endbr64