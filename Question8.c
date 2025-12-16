#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "
#define BUFFER_SIZE 1024
#define PROMPT_BUFFER_SIZE 128
#define MAX_ARGS 64

// Fonction de parsing

int parse_command(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \t\n");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return i;
}

// Gestion des redirections

void handle_redirections(char **args) {
    for (int i = 0; args[i] != NULL; i++) {

        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                write(STDERR_FILENO, "Erreur: nom de fichier manquant après >\n", 41);
                exit(EXIT_FAILURE);
            }

            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                write(STDERR_FILENO, "Erreur ouverture fichier\n", 26);
                exit(EXIT_FAILURE);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            break;
        }

        if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                write(STDERR_FILENO, "Erreur: nom de fichier manquant après <\n", 41);
                exit(EXIT_FAILURE);
            }

            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1) {
                write(STDERR_FILENO, "Erreur ouverture fichier\n", 26);
                exit(EXIT_FAILURE);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            break;
        }
    }
}

// PIPE FUNCTIONS

int find_pipe(char **args, int *pipe_pos) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            *pipe_pos = i;
            return 1;
        }
    }
    return 0;
}

void execute_pipe(char **args1, char **args2) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        handle_redirections(args1);
        execvp(args1[0], args1);
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        handle_redirections(args2);
        execvp(args2[0], args2);
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}



int main() {
    char buffer[BUFFER_SIZE];
    char prompt_buffer[PROMPT_BUFFER_SIZE];
    ssize_t bytes_read;

    int last_status = 0;
    int display_status = 0;
    long duration_ms = 0;

    struct timespec start, end;

    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));

    while (1) {
        
        if (display_status == 0)
            strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
        else if (display_status == 1)
            snprintf(prompt_buffer, PROMPT_BUFFER_SIZE,
                     "enseash [exit:%d|%ldms] %% ", last_status, duration_ms);
        else
            snprintf(prompt_buffer, PROMPT_BUFFER_SIZE,
                     "enseash [sign:%d|%ldms] %% ", last_status, duration_ms);

        write(STDOUT_FILENO, prompt_buffer, strlen(prompt_buffer));

        // Lecture 
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nBye bye...\n", 12);
            break;
        }

        buffer[bytes_read - 1] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }

        char *args[MAX_ARGS];
        parse_command(buffer, args);

        clock_gettime(CLOCK_MONOTONIC, &start);

        int pipe_pos;
        if (find_pipe(args, &pipe_pos)) {
            args[pipe_pos] = NULL;
            char **args1 = args;
            char **args2 = &args[pipe_pos + 1];

            execute_pipe(args1, args2);

            last_status = 0;
            display_status = 1;
        } else {
            pid_t pid = fork();

            if (pid == 0) {
                handle_redirections(args);
                execvp(args[0], args);
                write(STDERR_FILENO, "Erreur commande\n", 16);
                exit(EXIT_FAILURE);
            } else {
                int status;
                wait(&status);

                if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                    display_status = 1;
                } else if (WIFSIGNALED(status)) {
                    last_status = WTERMSIG(status);
                    display_status = 2;
                }
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        duration_ms = (end.tv_sec - start.tv_sec) * 1000 +
                      (end.tv_nsec - start.tv_nsec) / 1000000;
    }

    return 0;
}
