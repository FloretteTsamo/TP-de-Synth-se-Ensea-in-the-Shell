# TP-de-Synth-se-Ensea-in-the-Shell

Informations générales
Projet : EnseaSH - Micro Shell ENSEA
Cours : Programmation Système - 2ème année Majeure Informatique
Étudiants : Tom PICQUART et TSAMO Florette Miroslava
Date : Décembre 2024
Encadrants : MONCHAL Laurent, N. PAPAZOGLOU

Présentation du projet
Micro-shell en C développé dans le cadre du TP de Synthèse de Programmation Système à l'ENSEA. Implémente les fonctionnalités essentielles d'un shell UNIX : exécution de commandes, redirections, pipes et processus en arrière-plan.
Pour la première séance nous avons développé un micro-shell en langage C, appelé enseash, capable d'exécuter des commandes, d'afficher leurs codes de sortie et leurs temps d'exécution.
Le shell implémente progressivement les fonctionnalités suivantes :

  Affichage d'un message d'accueil et d'un prompt
  Lecture et exécution de commandes simples (REPL)
  Gestion de la sortie avec exit et Ctrl+D
  Affichage du code de retour ou du signal
  Mesure du temps d'exécution

Question 1 : Affichage du message d'accueil
Objectif
Afficher un message de bienvenue au lancement du shell, suivi d'un prompt simple.
Mise en œuvre
Nous utilisons l'appel système write() pour afficher :
Le message de bienvenue
Le prompt enseash %
Question 2 : Exécution de commandes simples
Objectif
Implémenter une boucle REPL (Read-Eval-Print Loop) permettant de :

Lire une commande saisie par l'utilisateur
Exécuter cette commande
Revenir au prompt
<img width="418" height="153" alt="image" src="https://github.com/user-attachments/assets/910bbd69-d79c-490d-9895-01655d016682" />

Concepts utilisés
fork()
L'appel système fork() crée un processus fils qui est une copie du processus père. Il retourne :

0 dans le processus fils
Le PID du fils dans le processus père
-1 en cas d'erreur

execvp()
execvp() remplace le code du processus courant par celui d'un nouveau programme. Si l'appel réussit, le code après execvp() n'est jamais exécuté.
wait()
wait() bloque le processus père jusqu'à ce qu'un de ses fils se termine. Cela évite de créer des processus zombies.

RESULTATS:
Difficultés rencontrées

Gestion du '\n' : Il faut bien penser à retirer le caractère de retour à la ligne lu par read().
Tableau d'arguments : execvp() nécessite un tableau terminé par NULL.

Question 3 : Gestion de la sortie du shell
Objectif
Permettre à l'utilisateur de quitter le shell proprement via :

La commande exit
Le raccourci clavier Ctrl+D (EOF)

Difficultés rencontrées

Comprendre que read() retourne 0 ou une valeur négative lors d'un EOF.
Utiliser strncmp() au lieu de strcmp() pour plus de sécurité.

Question 4 : Affichage du code de retour
Objectif
Afficher dans le prompt le code de retour de la commande précédente :

[exit:X] si terminaison normale (où X est le code de sortie)
[sign:Y] si terminaison par signal (où Y est le numéro du signal)

Concepts utilisés
Macros de sys/wait.h

WIFEXITED(status) : Retourne vrai si le processus s'est terminé normalement (via exit() ou return)
WEXITSTATUS(status) : Retourne le code de sortie (0-255)
WIFSIGNALED(status) : Retourne vrai si le processus a été terminé par un signal
WTERMSIG(status) : Retourne le numéro du signal qui a causé la terminaison

Difficultés rencontrées

Utilisation de snprintf() : Bien que le sujet interdise printf(), snprintf() est nécessaire pour formater des chaînes. Nous aurions pu implémenter notre propre fonction de formatage, mais cela aurait considérablement alourdi le code.
Gestion des signaux : Comprendre la différence entre une terminaison normale et une terminaison par signal.
Signal SIGINT : Le signal 2 (SIGINT) est envoyé par Ctrl+C.

Question 5 : Mesure du temps d'exécution
Objectif
Afficher le temps d'exécution de chaque commande dans le prompt, en millisecondes.
Format attendu :

[exit:X|Yms] pour une terminaison normale
[sign:X|Yms] pour une terminaison par signal

Concepts utilisés
clock_gettime()
Cette fonction permet d'obtenir l'heure actuelle avec une précision nanoseconde.

Analyse des résultats

Les commandes rapides comme date prennent quelques millisecondes (3-5ms)
sleep 2 prend bien environ 2000ms
Lorsqu'on interrompt avec Ctrl+C, le temps affiché correspond au temps écoulé avant l'interruption

Difficultés rencontrées

Précision du temps : Comprendre la différence entre tv_sec et tv_nsec, et bien faire la conversion en millisecondes.
Placement des mesures : Il faut mesurer le temps avant le fork() et après le wait() pour inclure tout le temps d'exécution du processus fils.
Overflow potentiel : Avec de longues durées, le calcul tv_nsec / 1000000 nécessite un long pour éviter les dépassements.

## 🛠️ Compilation et Installation

### Prérequis
- GCC (GNU Compiler Collection)
- Make
- Linux/Unix

### Compilation
```bash
# Cloner le dépôt
git clone https://github.com/votre-username/enseash.git
cd enseash

# Compiler
make

# Ou directement avec gcc
gcc -Wall -Wextra -std=c99 -o enseash main.c
```

### Exécution
```bash
./enseash
```

### Nettoyage
```bash
make clean
```
