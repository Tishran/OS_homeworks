#pragma once

#include <stdbool.h>

struct Context {
    void *rbx, *rbp, *r12, *r13, *r14, *r15;
    void *rsp, *rip;
    void *rdi, *rsi; //for arguments
    struct Context *next;
    struct Context *prev;
    bool toDel;
};

struct ContextList {
    struct Context *head;
    struct Context *tail;
    struct Context *curr;
};

extern void StartFiber(struct Context *newContext);

extern void SwitchContext(struct Context *current, struct Context *next);

void Handler(void (*func)(void *), void *data);

// Создаёт новый fiber и помещает его в конец очереди исполнения
//
// Input:
//   - func, data - аналогично аргументам pthread_create
void FiberSpawn(void (*func)(void *), void *data);

// Останавливает текущий fiber, помещает его в конце очереди исполнения
// Запускает первый fiber из очереди исполнения
void FiberYield();

// Возвращает 1, если текущий fiber единственный, иначе 0
int FiberTryJoin();
