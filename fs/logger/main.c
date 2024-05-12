#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>

const int DATA_SIZE = 4096;
volatile sig_atomic_t start_rotate = 0;
int count_files = 1;

void rename_log_files(const char *file_name, FILE **log_file) {
    fclose(*log_file);

    char old_name[strlen(file_name) + 16];
    char new_name[strlen(file_name) + 16];

    struct stat status;
    for (int i = count_files; i >= 1; i--) {
        strcpy(old_name, file_name);
        strcat(old_name, ".");
        char string_number1[100];
        sprintf(string_number1, "%d", i);
        strcat(old_name, string_number1);

        strcpy(new_name, file_name);
        strcat(new_name, ".");
        char string_number2[100];
        sprintf(string_number2, "%d", i + 1);
        strcat(new_name, string_number2);

        if (stat(old_name, &status) == 0) {
            rename(old_name, new_name);
        }
    }

    snprintf(new_name, sizeof(new_name), "%s.%d", file_name, 1);
    rename(file_name, new_name);
    ++count_files;
}

void sig_handler(int signum) {
    start_rotate = 1;
}

void check(const char* file_name, FILE** log_file) {
    if (start_rotate) {
        rename_log_files(file_name, log_file);
        *log_file = fopen(file_name, "w");
        start_rotate = 0;
    }
}

int main(int argc, char *argv[]) {
    const char *file_name = argv[1];
    FILE *log_file = fopen(file_name, "w");

    struct sigaction signal_action;
    signal_action.sa_handler = sig_handler;
    sigemptyset((sigset_t *) &signal_action.sa_mask);
    signal_action.sa_flags = SA_RESTART;

    sigaction(SIGHUP, &signal_action, NULL);

    char buffer[DATA_SIZE];

    while (!feof(log_file)) {
        check(file_name, &log_file);
        if (fgets(buffer, DATA_SIZE, stdin) == NULL) {
            break;
        }

        check(file_name, &log_file);
        fputs(buffer, log_file);
        check(file_name, &log_file);
    }

    fclose(log_file);

    return 0;
}