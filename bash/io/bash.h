#include "tokenizer.h"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

void copyStrFromToken(struct Token *token, char **result) {
    size_t string_size = (token->len + 1) * sizeof(char);
    *result = malloc(string_size);

    strncpy(*result, token->start, token->len);
    (*result)[string_size - 1] = '\0';
}

size_t getArgs(char ***argv, struct Token *token, char **in_file, char **out_file) {
    int out_count = 0;
    int in_count = 0;

    size_t curr_idx = 0;
    while (token != NULL) {
        if (token->type == TT_WORD) {
            copyStrFromToken(token, &(*argv)[curr_idx]);
            ++curr_idx;
        } else {
            if (token->next == NULL || token->next->type != TT_WORD) {
                return 0;
            }

            if (token->type == TT_INFILE && in_count == 0) {
                ++in_count;
                copyStrFromToken(token->next, in_file);
            } else if (token->type == TT_OUTFILE && out_count == 0) {
                ++out_count;
                copyStrFromToken(token->next, out_file);
            } else if ((token->type == TT_OUTFILE && out_count > 0) ||
                       (token->type == TT_INFILE && in_count > 0)) {
                return 0;
            }

            token = token->next;
        }

        token = token->next;
    }

    (*argv)[curr_idx] = (char *) NULL;
    *argv = (char **) realloc(*argv, sizeof(char *) * (curr_idx + 1));

    return curr_idx + 1;
}

bool switchIO(int fd, int std_fd) {
    if (fd == -1) {
        return false;
    }

    if (dup2(fd, std_fd) == -1) {
        perror("dup2(stdin)");
    }

    close(fd);
    return true;
}

void handleStdIO(int stdin_copy, int stdout_copy) {
    dup2(stdout_copy, STDOUT_FILENO);
    close(stdout_copy);

    dup2(stdin_copy, STDIN_FILENO);
    close(stdin_copy);
}

void freeAll(char ***argv, char **in_file, char **out_file) {
    if (*in_file != NULL) free(*in_file);
    if (*out_file != NULL) free(*out_file);

    size_t i = 0;
    while ((*argv)[i] != NULL) {
        free((*argv)[i]);
        ++i;
    }

    free((*argv));
}

void Exec(struct Tokenizer *tokenizer) {
    if (tokenizer->head == NULL) {
        return;
    }

    char *in_file = NULL;
    char *out_file = NULL;

    char **argv = (char **) malloc((tokenizer->token_count + 1) * sizeof(char *));
    for (int i = 0; i < tokenizer->token_count + 1; ++i) {
        argv[i] = NULL;
    }

    size_t argv_size = getArgs(&argv, tokenizer->head, &in_file, &out_file);
    if (argv_size == 0) {
        printf("Syntax error\n");
    } else {
        int stdin_copy = dup(STDIN_FILENO);
        int stdout_copy = dup(STDOUT_FILENO);

        bool fine = true;

        if (in_file != NULL) {
            int fd_in = open(in_file, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
            if (!switchIO(fd_in, STDIN_FILENO)) {
                printf("I/O error\n");
                fine = false;
            }
        }

        if (out_file != NULL) {
            int fd_out = open(out_file, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (!switchIO(fd_out, STDOUT_FILENO)) {
                printf("I/O error\n");
                fine = false;
            }
        }

        if (fine) {
            pid_t pid = fork();
            int status;
            switch (pid) {
                case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                case 0:
                    if (execvp(argv[0], argv) == -1) {
                        printf("Command not found\n");
                    }
                    break;
                default:
                    waitpid(pid, &status, 0);
                    if (!WIFEXITED(status)) {
                        perror(argv[0]);
                    }
            }
        }

        handleStdIO(stdin_copy, stdout_copy);
    }

    freeAll(&argv, &in_file, &out_file);
}