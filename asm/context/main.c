#include "context.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void TestSimple() {
    struct Context ctx;

    fprintf(stderr, "saving context\n");
    if (savectx(&ctx) == 0) {
        fprintf(stderr, "restoring context\n");
        jumpctx(&ctx);
        assert(!"unreachable");
    }
    fprintf(stderr, "context restored\n");
}

static void Set(int arr[7], int idx, int val) {
    assert(idx == val);
    assert(idx < 7);
    arr[idx] = val;
}

static void TestChain() {
    struct Context ctx_a;
    struct Context ctx_b;
    struct Context ctx_c;

    int res[7];
    volatile int i = 0;

    Set(res, i++, 0);
    if (savectx(&ctx_a) == 1) {
        Set(res, i++, 4);
        jumpctx(&ctx_b);
    }

    Set(res, i++, 1);
    if (savectx(&ctx_b) == 1) {
        Set(res, i++, 5);
        jumpctx(&ctx_c);
    }

    Set(res, i++, 2);
    if (savectx(&ctx_c) == 1) {
        Set(res, i++, 6);

        for (int j = 0; j < 7; ++j) {
            assert(res[j] == j);
        }
        return;
    } else {
        Set(res, i++, 3);
        jumpctx(&ctx_a);
    }

    assert(!"unreachable");
}

static void TestLoop() {
    volatile int i = 0;
    int j = 0;
    struct Context ctx;

    fprintf(stderr, "saving ctx\n");
    if (savectx(&ctx) == 0) {
        fprintf(stderr, "restoring ctx %d\n", i);
        jumpctx(&ctx);
    } else if (i == 10) {
        assert(j == 0);
        return;
    } else {
        i++;
        j++;
        fprintf(stderr, "restoring ctx %d\n", i);
        jumpctx(&ctx);
    }

    assert(!"unreachable");
}

static __attribute__((noinline)) void RestoreImpl(struct Context* ctx) {
    volatile int i = 0;
    jumpctx(ctx);
    assert(!"unreachable");
}

static __attribute__((noinline)) void Restore(struct Context* ctx) {
    volatile int i = 0;
    RestoreImpl(ctx);
}

static intptr_t TestFunction1() {
    volatile int j = 5;
    struct Context ctx;
    if (savectx(&ctx) == 0) {
        Restore(&ctx);
    }
    assert(j == 5);
    return (intptr_t)TestFunction1;
}

static intptr_t TestFunction2() {
    TestFunction1();
    return (intptr_t)TestFunction2;
}

int main() {
    TestSimple();
    TestChain();
    TestLoop();
    assert(TestFunction1() == (intptr_t)TestFunction1);
    assert(TestFunction2() == (intptr_t)TestFunction2);
    return 0;
}
