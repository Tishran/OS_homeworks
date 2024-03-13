#include "syscall.h"

int open(const char* pathname, int flags) {
    int result = 0;

    asm("mov $2, %%eax\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%esi\n\t"
        "syscall\n\t"
        "mov %%eax, %0\n\t"
        : "=r" (result)
        : "rm" (pathname), "rm" (flags)
        : "%rax", "%rdi", "%rsi");

    return result;
}

ssize_t read(int fd, void* buf, size_t count) {
    ssize_t len = 0;

    asm("mov $0, %%eax\n\t"
        "mov %1, %%edi\n\t"
        "mov %2, %%rsi\n\t"
        "mov %3, %%rdx\n\t"
        "syscall\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (len)
        : "rm" (fd), "r" (buf), "rm" (count)
        : "%rax", "%rdi", "%rdx");

    return len;
}

ssize_t write(int fd, const void* buf, ssize_t count) {
    ssize_t len = 0;

    asm("mov $1, %%eax\n\t"
        "mov %1, %%edi\n\t"
        "mov %2, %%rsi\n\t"
        "mov %3, %%rdx\n\t"
        "syscall\n\t"
        "mov %%rax, %0\n\t"
        : "=r" (len)
        : "rm" (fd), "r" (buf), "rm" (count)
        : "%rax", "%rdi", "%rdx");

    return len;
}

int pipe(int pipefd[2]) {
    int result;

    asm("mov $22, %%eax\n\t"
        "mov %1, %%rdi\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (result)
        : "rm" (pipefd)
        : "%rax", "%rdi");

    return result;
}

int close(int fd) {
    int result;

    asm("mov $3, %%eax\n\t"
        "mov %1, %%rdi\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (result)
        : "rm" (fd)
        : "%rax", "%rdi");

    return result;
}

int dup(int oldfd) {
    int result;

    asm("mov $32, %%eax\n\t"
        "mov %1, %%rdi\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (result)
        : "rm" (oldfd)
        : "%rax", "%rdi");

    return result;
}

void exit(int status) {
    asm("mov $60, %%eax\n\t"
        "mov %0, %%rdi\n\t"
        "syscall\n\t"
        :
        : "rm" (status)
        : "%rax", "%rdi");
}

pid_t fork() {
    pid_t pid;

    asm("mov $57, %%eax\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (pid)
        :
        : "%rax");

    return pid;
}

pid_t waitpid(pid_t pid, int* wstatus, int options) {
    pid_t res_pid;

    asm("mov $61, %%eax\n\t"
        "mov %1, %%edi\n\t"
        "mov %2, %%rsi\n\t"
        "mov %3, %%edx\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (res_pid)
        : "rm" (pid), "rm" (wstatus), "rm" (options)
        : "%rax", "rsi", "rdx");

    return res_pid;
}

int execve(const char* filename, char* const argv[], char* const envp[]) {
    int res;

    asm("mov $59, %%eax\n\t"
        "mov %1, %%rdi\n\t"
        "mov %2, %%rsi\n\t"
        "mov %3, %%rdx\n\t"
        "syscall\n\t"
        "mov %%eax, %0"
        : "=r" (res)
        : "rm" (filename), "rm" (argv), "rm" (envp)
        : "%rax", "rsi", "rdx");

    return res;
}
