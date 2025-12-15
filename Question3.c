#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "
#define BUFFER_SIZE 1024


int main() {
    char buffer[BUFFER_SIZE]; // commande saisie
    ssize_t bytes_read; // nombre d'octets lus
    int last_status = 0; // statut de la dernière commande exécutée
    int display_status = 0; // 0 = pas d'affichage, 1 = exit, 2 = signal
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));
    
    while (1) {
        write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
        
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        
        //  Ctrl+D (EOF)
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nBye bye...\n", 12);
            break;
        }
        
        buffer[bytes_read - 1] = '\0';
        
        //  commande "exit"
        if (strncmp(buffer, "exit", 4) == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }
        
        pid_t pid = fork();
        
        if (pid == -1) {
            write(STDERR_FILENO, "Erreur fork\n", 12);
        } else if (pid == 0) {
            char *args[] = {buffer, NULL}; // tableau des commandes ls...
            execvp(buffer, args); // exécution des commandes
            exit(EXIT_FAILURE);
        } else {
            int status;
            wait(&status);
        }
    }
    
    return 0;
}