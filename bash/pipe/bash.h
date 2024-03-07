#include "tokenizer.h"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

int countPipes(struct Token *token) {
    int res = 0;
    while (token != NULL) {
        if (token->type == TT_PIPE) {
            ++res;
        }

        token = token->next;
    }

    return res;
}

void copyStrFromToken(struct Token *token, char **result) {
    size_t string_size = (token->len + 1) * sizeof(char);
    *result = malloc(string_size);

    strncpy(*result, token->start, token->len);
    (*result)[string_size - 1] = '\0';
}


int countWordsPerCmd(struct Token *token, size_t **res, char **in_file, char **out_file) {
    int cmds = countPipes(token) + 1;
    (*res) = malloc(cmds * sizeof(size_t));

    size_t cnt_words = 0;
    size_t curr_cmd_idx = 0;

    size_t out_count = 0;
    size_t in_count = 0;
    size_t in_count_within_pipe = 0;
    while (token != NULL) {
        if (token->type == TT_WORD) {
            ++cnt_words;
        } else if (token->type == TT_PIPE) {
            if (token->next == NULL || token->next->type == TT_PIPE) {
                return -1;
            }

            if (out_count != 0 || (curr_cmd_idx != 0 && in_count_within_pipe != 0)) {
                return -1;
            }

            (*res)[curr_cmd_idx] = cnt_words;
            ++curr_cmd_idx;
            cnt_words = 0;
            in_count_within_pipe = 0;
        } else {
            if (token->next == NULL || token->next->type != TT_WORD) {
                return -1;
            }

            if (token->type == TT_INFILE && in_count == 0) {
                ++in_count;
                ++in_count_within_pipe;
                copyStrFromToken(token->next, in_file);
            } else if (token->type == TT_OUTFILE && out_count == 0) {
                ++out_count;
                copyStrFromToken(token->next, out_file);
            } else if ((token->type == TT_OUTFILE && out_count > 0) ||
                       (token->type == TT_INFILE && in_count > 0)) {
                return -1;
            }

            token = token->next;
        }

        token = token->next;
    }

    (*res)[curr_cmd_idx] = cnt_words;

    return cmds;
}

void getArgs(char ***argv, struct Token **token) {
    size_t curr_idx = 0;
    while ((*token) != NULL) {
        if ((*token)->type == TT_PIPE) {
            (*token) = (*token)->next;
            break;
        } else if ((*token)->type == TT_WORD) {
            copyStrFromToken(*token, &(*argv)[curr_idx]);
            ++curr_idx;
        } else {
            (*token) = (*token)->next;
        }

        (*token) = (*token)->next;
    }

    (*argv)[curr_idx] = (char *) NULL;
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

void restoreStdIO(int stdin_copy, int stdout_copy) {
    dup2(stdout_copy, STDOUT_FILENO);
    close(stdout_copy);

    dup2(stdin_copy, STDIN_FILENO);
    close(stdin_copy);
}

void freeArgv(char ***argv) {
    size_t i = 0;
    while ((*argv)[i] != NULL) {
        free((*argv)[i]);
        ++i;
    }

    free((*argv));
}

void freeAll(size_t **cnt_words_per_command, char **in_file, char **out_file) {
    if (*in_file != NULL)
        free(*in_file);

    if (*out_file != NULL)
        free(*out_file);

    if (*cnt_words_per_command != NULL)
        free(*cnt_words_per_command);
}

void Exec(struct Tokenizer *tokenizer) {
    struct Token *head = tokenizer->head;
    if (head == NULL) {
        return;
    }

    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);

    char *in_file = NULL;
    char *out_file = NULL;

    size_t *cnt_words_per_command;
    int cnt_cmds = countWordsPerCmd(head, &cnt_words_per_command, &in_file, &out_file);

    if (cnt_cmds == -1) {
        printf("Syntax error\n");
        restoreStdIO(stdin_copy, stdout_copy);
        freeAll(&cnt_words_per_command, &in_file, &out_file);
        return;
    }

    if (in_file != NULL) {
        int fd_in = open(in_file, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if (!switchIO(fd_in, STDIN_FILENO)) {
            printf("I/O error\n");
            restoreStdIO(stdin_copy, stdout_copy);
            freeAll(&cnt_words_per_command, &in_file, &out_file);
            return;
        }
    }

    if (out_file != NULL) {
        int fd_out = open(out_file, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        if (!switchIO(fd_out, STDOUT_FILENO)) {
            printf("I/O error\n");
            restoreStdIO(stdin_copy, stdout_copy);
            freeAll(&cnt_words_per_command, &in_file, &out_file);
            return;
        }
    }

    int pipefd[2] = {-1, -1};
    int prevfd[2] = {STDIN_FILENO, STDOUT_FILENO};
    for (size_t curr_cmd_idxs = 0; curr_cmd_idxs < cnt_cmds; ++curr_cmd_idxs) {
        bool hasPipe = (cnt_cmds - curr_cmd_idxs > 1);

        if (hasPipe) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        char **argv = (char **) malloc((cnt_words_per_command[curr_cmd_idxs] + 1) * sizeof(char *));
        for (int i = 0; i < cnt_words_per_command[curr_cmd_idxs] + 1; ++i) {
            argv[i] = NULL;
        }

        getArgs(&argv, &head);

        pid_t pid = fork();
        int status;
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(pipefd[0]);
            if (hasPipe) {
                if (!switchIO(pipefd[1], prevfd[1])) {
                    fprintf(stderr, "switchIO error\n");
                    freeArgv(&argv);
                    break;
                }
            }

            if (execvp(argv[0], argv) == -1) {
                dprintf(stdout_copy, "Command not found\n");
                freeArgv(&argv);
                break;
            }
        } else {
            close(pipefd[1]);
            if (hasPipe) {
                if (!switchIO(pipefd[0], prevfd[0])) {
                    fprintf(stderr, "switchIO error\n");
                    freeArgv(&argv);
                    break;
                }
            }

            waitpid(pid, &status, 0);
            if (!WIFEXITED(status)) {
                perror(argv[0]);
            }
        }

        if (!hasPipe) {
            if (pipefd[0] != -1) close(pipefd[0]);
            if (pipefd[1] != -1) close(pipefd[1]);
        }

        freeArgv(&argv);
    }

    restoreStdIO(stdin_copy, stdout_copy);
    freeAll(&cnt_words_per_command, &in_file, &out_file);
}
