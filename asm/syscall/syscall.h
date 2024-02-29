typedef long ssize_t;
typedef unsigned long size_t;

typedef int pid_t;

// Opens the file specified by pathname.
int open(const char* pathname, int flags);

// Closes a file descriptor, so that it no longer
// refers to any file and may be reused.
int close(int fd);

// Attempts to read up to count bytes from file
// descriptor fd into the buffer starting at buf.
ssize_t read(int fd, void* buf, size_t count);

// Writes up to count bytes from the buffer starting at
// buf to the file referred to by the file descriptor fd.
ssize_t write(int fd, const void* buf, ssize_t count);

// Creates a pipe, a unidirectional data channel
// that can be used for interprocess communication.
int pipe(int pipefd[2]);

// Creates a copy of the file descriptor oldfd, using the
// lowest-numbered unused file descriptor for the new descriptor.
int dup(int oldfd);

// Creates a new process by duplicating the calling process.
pid_t fork();

// Wait for state changes in a child of the calling process.
pid_t waitpid(pid_t pid, int* wstatus, int options);

// Executes the program pointed to by filename.
int execve(const char* filename, char* const argv[], char* const envp[]);

// Terminates the calling process.
void exit(int status);
