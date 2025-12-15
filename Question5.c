#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "
#define BUFFER_SIZE 1024
#define PROMPT_BUFFER_SIZE 128

int main() {
    char buffer[BUFFER_SIZE];
    char prompt_buffer[PROMPT_BUFFER_SIZE];
    ssize_t bytes_read;
    int last_status = 0;
    int display_status = 0;  // 0 = pas d'affichage, 1 = exit, 2 = signal
    long duration_ms = 0;
    struct timespec start, end;
    
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));
    
    while (1) {
        
        if (display_status == 0) {
            strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
        } else if (display_status == 1) {
            
            int len = snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                              "enseash [exit:%d|%ldms] %% ", last_status, duration_ms);
            if (len < 0 || len >= PROMPT_BUFFER_SIZE) {
                strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
            }
        } else {
            
            int len = snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                              "enseash [sign:%d|%ldms] %% ", last_status, duration_ms);
            if (len < 0 || len >= PROMPT_BUFFER_SIZE) {
                strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
            }
        }
        
        write(STDOUT_FILENO, prompt_buffer, strlen(prompt_buffer));
        
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nBye bye...\n", 12);
            break;
        }
        
        buffer[bytes_read - 1] = '\0';
        
        if (strncmp(buffer, "exit", 4) == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        pid_t pid = fork();
        
        if (pid == -1) {
            write(STDERR_FILENO, "Erreur fork\n", 12);
            continue;
        } else if (pid == 0) {
            char *args[] = {buffer, NULL};
            execvp(buffer, args);
            
            // Si execvp échoue
            write(STDERR_FILENO, "Erreur commande\n", 16);
            exit(EXIT_FAILURE);
        } else {
            
            int status;
            wait(&status);
        
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            duration_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                         (end.tv_nsec - start.tv_nsec) / 1000000;
            
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
                display_status = 1;
            } else if (WIFSIGNALED(status)) {
                last_status = WTERMSIG(status);
                display_status = 2;
            }
        }
    }
    
    return 0;
}