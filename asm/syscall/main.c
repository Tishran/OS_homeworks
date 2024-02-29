#include "syscall.h"

int TestExec() {
    int pipefd[2];
    pipe(pipefd);

    pid_t p = fork();
    if (p == 0) {
        char* argv[] = {"/bin/echo", "hi", 0};
        char* envp[] = {0};
        close(pipefd[0]);
        close(1);
        dup(pipefd[1]);
        execve("/bin/echo", argv, envp);
        exit(1);
    }
    close(pipefd[1]);

    char buf[64];
    ssize_t r = read(pipefd[0], buf, sizeof(buf));
    if (r != 3 || buf[0] != 'h' || buf[1] != 'i' || buf[2] != '\n') {
        return 1;
    }

    int status;
    pid_t s = waitpid(p, &status, 0);
    if (s != p || (status & 0x7F) != 0) {
        return 1;
    }
    return 0;
}

int TestFork() {
    pid_t p = fork();
    if (p == 0) {
        exit(0);
    }
    int status;
    pid_t r = waitpid(p, &status, 0);
    if (r != p || (status & 0x7F) != 0 || ((status >> 8) & 0xFF) != 0) {
        return 1;
    }
    return 0;
}

int TestPipe() {
    int pipefd[2];
    pipe(pipefd);
    write(pipefd[1], "test", 4);

    char buf[64];
    ssize_t r = read(pipefd[0], buf, 4);
    if (r != 4 || buf[0] != 't' || buf[1] != 'e' || buf[2] != 's' || buf[3] != 't') {
        return 1;
    }
    return 0;
}

int TestRead() {
    char buf[64];
    int fd = open("data.txt", 0);
    ssize_t r = read(fd, buf, sizeof(buf));
    if (r != 6 || buf[0] != 'h' || buf[1] != 'e' || buf[2] != 'l' || buf[3] != 'l' || buf[4] != 'o' || buf[5] != '\n') {
        return 1;
    }
    return 0;
}

int main() {
    int res = 0;
    res |= TestExec();
    res |= TestFork();
    res |= TestPipe();
    res |= TestRead();
    return res;
}
