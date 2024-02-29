#include "syscall.h"

int open(const char* pathname, int flags) {
    // Your code here
}

ssize_t read(int fd, void* buf, size_t count) {
    // Your code here
}

ssize_t write(int fd, const void* buf, ssize_t count) {
    // Your code here
}

int pipe(int pipefd[2]) {
    // Your code here
}

int close(int fd) {
    // Your code here
}

int dup(int oldfd) {
    // Your code here
}

void exit(int status) {
    // Your code here
}

pid_t fork() {
    // Your code here
}

pid_t waitpid(pid_t pid, int* wstatus, int options) {
    // Your code here
}

int execve(const char* filename, char* const argv[], char* const envp[]) {
    // Your code here
}
