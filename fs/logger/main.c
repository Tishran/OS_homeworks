#include <stdio.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

volatile sig_atomic_t start_rotate = 0;

int extract_number_from_filename(const char *filename, const char *prefix) {
    int n = -1;
    if (strncmp(filename, prefix, strlen(prefix)) == 0) {
        const char *number_str = filename + strlen(prefix);
        n = atoi(number_str);
    }
    return n;
}

void rename_log_files(const char *file_name, FILE *log_file) {
    fclose(log_file);

    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    char old_name[256];
    char new_name[256];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strncmp(entry->d_name, file_name, strlen(file_name)) == 0 &&
                strcmp(entry->d_name, file_name) != 0) {
                int number = extract_number_from_filename(entry->d_name, file_name);

                snprintf(old_name, sizeof(old_name), "%s.%d", file_name, number);
                snprintf(new_name, sizeof(new_name), "%s.%d", file_name, number + 1);

                rename(old_name, new_name);
            }
        }
    }

    closedir(dir);

    snprintf(new_name, sizeof(new_name), "%s.%d", file_name, 1);
    rename(file_name, new_name);
}

void sighup_handler(int signum) {
    start_rotate = 1;
}

int main(int argc, char *argv[]) {
    const char *file_name = argv[1];
    FILE *log_file = fopen(file_name, "w+");

    struct sigaction signal_action;
    signal_action.sa_handler = sighup_handler;
    sigemptyset((sigset_t *) &signal_action.sa_mask);
    signal_action.sa_flags = SA_RESTART;

    sigaction(SIGHUP, &signal_action, NULL);

    char buffer[4096];

    while (!feof(log_file)) {
        if (start_rotate) {
            rename_log_files(file_name, log_file);
            log_file = fopen(file_name, "w+");
            start_rotate = 0;
        }

        fputs(buffer, log_file);
    }

    fclose(log_file);

    return 0;
}