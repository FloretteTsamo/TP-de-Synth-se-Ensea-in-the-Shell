
#include <string.h>
#include <unistd.h>

#define WELCOME_MSG "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define PROMPT "enseash % "

int main() {
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT)); // strlen pour le nombre de caractères à écrire et éviter le '\0' et non sizeof qui donnerait la taille totale du tableau (1024)
return 0;
}