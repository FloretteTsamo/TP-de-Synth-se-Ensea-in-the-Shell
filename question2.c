#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash $ "
#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));
    
    while (1) {
        write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
        
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1); // Lecture de la commande
        if (bytes_read <= 0) {
            break;
        }
        
        buffer[bytes_read - 1] = '\0'; // Retirer le '\n' final
        
        pid_t pid = fork();
        
        if (pid == -1) {
            // Erreur lors du fork
            write(STDERR_FILENO, "Erreur fork\n", 12);
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // fils 
            char *args[] = {buffer, NULL};
            execvp(buffer, args);
            
            //  execvp a échoué
            write(STDERR_FILENO, "Erreur execvp\n", 14);
            exit(EXIT_FAILURE);
        } else {
            // père s
            int status;
            wait(&status);
        }
    }
    
    return 0;
}