#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "
#define BUFFER_SIZE 1024
#define PROMPT_BUFFER_SIZE 64

int main() {
    char buffer[BUFFER_SIZE];
    char prompt_buffer[PROMPT_BUFFER_SIZE];
    ssize_t bytes_read;
    int last_status = 0;
    int display_status = 0; // 0 = pas d'affichage, 1 = exit, 2 = signal
    
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));
    
    while (1) {
        // Construction du prompt en fonction du statut précédent
        if (display_status == 0) {
            strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
        } else if (display_status == 1) {
            // Terminaison normale : afficher exit code
            int len = snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                              "enseash [exit:%d] %% ", last_status);
            if (len < 0) strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
        } else {
            // Terminaison par signal
            int len = snprintf(prompt_buffer, PROMPT_BUFFER_SIZE, 
                              "enseash [sign:%d] %% ", last_status);
            if (len < 0) strncpy(prompt_buffer, PROMPT, PROMPT_BUFFER_SIZE);
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
        
        pid_t pid = fork();
        
        if (pid == -1) {
            write(STDERR_FILENO, "Erreur fork\n", 12);
        } else if (pid == 0) {
            char *args[] = {buffer, NULL};
            execvp(buffer, args);
            exit(EXIT_FAILURE);
        } else {
            int status;
            wait(&status);
            
            // Analyse du statut de terminaison
            if (WIFEXITED(status)) {
                // Terminaison normale
                last_status = WEXITSTATUS(status);
                display_status = 1;
            } else if (WIFSIGNALED(status)) {
                // Terminaison par signal
                last_status = WTERMSIG(status);
                display_status = 2;
            }
        }
    }
    
    return 0;
}