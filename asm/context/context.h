struct Context {
    void* rip;
    void* rsp;
    void* rbp;
};

int savectx(struct Context* ctx);
__attribute__((noreturn)) void jumpctx(struct Context* ctx);
