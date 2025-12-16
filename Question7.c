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

// Fonction de parsing (héritée de la question 6)
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

//  Gestion des redirections
void handle_redirections(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            // Redirection de stdout vers fichier
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
            args[i] = NULL;  // Retirer '>' et le nom de fichier
            break;
            
        } else if (strcmp(args[i], "<") == 0) {
            // Redirection de stdin depuis fichier
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
        // Construction du prompt
        if (display_status == 0) {
            strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
        } else if (display_status == 1) {
            snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                    "enseash [exit:%d|%ldms] %% ", last_status, duration_ms);
        } else {
            snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                    "enseash [sign:%d|%ldms] %% ", last_status, duration_ms);
        }
        
        write(STDOUT_FILENO, prompt_buffer, strlen(prompt_buffer));
        
        // Lecture de la commande
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
        
        // Mesure du temps : début
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        pid_t pid = fork();
        
        if (pid == -1) {
            write(STDERR_FILENO, "Erreur fork\n", 12);
            continue;
        } else if (pid == 0) {
            // Processus fils
            char *args[MAX_ARGS];
            parse_command(buffer, args);
            
            //Gérer les redirections AVANT execvp
            handle_redirections(args);
            
            if (args[0] != NULL) {
                execvp(args[0], args);
            }
            
            write(STDERR_FILENO, "Erreur commande\n", 16);
            exit(EXIT_FAILURE);
        } else {
            // Processus père
            int status;
            wait(&status);
            
            // Mesure du temps : fin
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