#include "tokenizer.h"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


char *copyStrFromToken(struct Token *token) {
    size_t string_size = token->len * sizeof(char) + 1;
    char *result = malloc(string_size);

    strncpy(result, token->start, token->len);
    result[string_size - 1] = '\0';

    return result;
}

void Exec(struct Tokenizer *tokenizer) {
    struct Token *curr_token = tokenizer->head;

    if (curr_token == NULL) {
        return;
    }

    char *argv[tokenizer->token_count + 1];

    size_t curr_idx = 0;
    while (curr_token != NULL) {
        argv[curr_idx] = copyStrFromToken(curr_token);

        ++curr_idx;
        curr_token = curr_token->next;
    }

    argv[tokenizer->token_count] = NULL;

    pid_t pid = fork();
    int status;
    switch (pid) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            if (execvp(argv[0], argv) == -1) {
                printf("%s", "Command not found\n");
            }
        default:
            waitpid(pid, &status, 0);
            if (!WIFEXITED(status)) {
                perror(argv[0]);
            }

            for (int i = 0; i <= tokenizer->token_count; ++i) {
                free(argv[i]);
            }
    }
}
