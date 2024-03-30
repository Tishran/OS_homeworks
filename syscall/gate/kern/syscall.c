#include <inc/stdio.h>
#include <kern/syscall.h>

void sys_work(void *p)
{
    char* c = (char*) p;
    while (*c) {
        cprintf("%c", *c ^ 0x5a);
        c++;
    }
}

void sys_retire()
{
    while (1);
}
