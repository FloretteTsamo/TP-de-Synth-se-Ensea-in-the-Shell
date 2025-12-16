# TP de Synthèse - Ensea in the Shell

##  Informations générales

**Projet :** EnseaSH - Micro Shell ENSEA  
**Cours :** Programmation Système - 2ème année Majeure Informatique  
**Étudiants :** Tom PICQUART et TSAMO Florette Miroslava  
**Date :** Décembre 2024  
**Encadrants :** MONCHAL Laurent, N. PAPAZOGLOU

---

##  Présentation du projet

Micro-shell en C développé dans le cadre du TP de Synthèse de Programmation Système à l'ENSEA. Implémente les fonctionnalités essentielles d'un shell UNIX : exécution de commandes, redirections, pipes et processus en arrière-plan.

Pour la première séance, nous avons développé un micro-shell en langage C, appelé **enseash**, capable d'exécuter des commandes, d'afficher leurs codes de sortie et leurs temps d'exécution.

Le shell implémente progressivement les fonctionnalités suivantes :
-  Affichage d'un message d'accueil et d'un prompt
-  Lecture et exécution de commandes simples (REPL)
-  Gestion de la sortie avec `exit` et `Ctrl+D`
-  Affichage du code de retour ou du signal


---

## Question 1 : Affichage du message d'accueil

###  Objectif
Afficher un message de bienvenue au lancement du shell, suivi d'un prompt simple.

###  Mise en œuvre
Voir le fichier [`question1.c`](TP1.c)

Nous utilisons l'appel système `write()` (et non `printf()`) pour afficher :
- Le message de bienvenue : `"Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"`
- Le prompt simple : `"enseash % "`

Les constantes sont définies avec `#define` pour éviter les "magic strings".

###  Résultat

<img width="902" height="47" alt="image" src="https://github.com/user-attachments/assets/372485c0-8a80-4b8a-92b4-784271a9efc7" />

Le shell affiche correctement le message d'accueil suivi du prompt.

###  Difficultés rencontrées
Aucune difficulté particulière pour cette première étape.

---

## Question 2 : Exécution de commandes simples

###  Objectif
Implémenter une boucle REPL (Read-Eval-Print Loop) permettant de :
- Lire une commande saisie par l'utilisateur
- Exécuter cette commande
- Revenir au prompt

###  Mise en œuvre
Voir le fichier [`question2.c`](question2.c)

#### Concepts utilisés

**`fork()`** : Crée un processus fils (copie du père)
- Retourne `0` dans le fils
- Retourne le PID du fils dans le père
- Retourne `-1` en cas d'erreur

**`execvp()`** : Remplace le code du processus par un nouveau programme. Le code après `execvp()` n'est jamais exécuté si l'appel réussit.

**`wait()`** : Bloque le père jusqu'à la terminaison d'un fils, évitant ainsi les processus zombies.

#### Architecture
```
Boucle infinie
  ├─ Afficher le prompt
  ├─ Lire la commande (read)
  ├─ fork()
  │   ├─ Fils : execvp(commande)
  │   └─ Père : wait()
  └─ Retour au début de la boucle
```

### Résultat

<img width="765" height="215" alt="image" src="https://github.com/user-attachments/assets/250af448-75a7-4e92-8a79-104e6bbb51b5" />

Les commandes `date`, `hostname`, et `pwd` s'exécutent correctement et le shell revient au prompt après chaque exécution.

###  Difficultés rencontrées
- **Gestion du `\n`** : Il faut retirer le caractère de retour à la ligne lu par `read()` en faisant `buffer[bytes_read - 1] = '\0'`
- **Tableau d'arguments** : `execvp()` nécessite un tableau terminé par `NULL` : `char *args[] = {buffer, NULL};`
- **Gestion d'erreur** : Vérifier que `fork()` ne retourne pas `-1`

---

## Question 3 : Gestion de la sortie du shell

###  Objectif
Permettre à l'utilisateur de quitter le shell proprement via :
- La commande `exit`
- Le raccourci clavier `Ctrl+D` (EOF)

###  Mise en œuvre
Voir le fichier [`TP3.C`](TP3.c)

Deux mécanismes de sortie sont implémentés :

1. **Détection de EOF (Ctrl+D)** : `read()` retourne `0` ou une valeur négative
2. **Commande "exit"** : Comparaison avec `strncmp(buffer, "exit", 4)`

Dans les deux cas, un message `"Bye bye...\n"` est affiché avant la terminaison.

###  Résultat

Sortie exit


<img width="768" height="273" alt="image" src="https://github.com/user-attachments/assets/ec30b723-25a1-43d8-abd9-9ea5e02a6672" />




Sortie Crtl+D


<img width="757" height="292" alt="image" src="https://github.com/user-attachments/assets/11ead1c2-5842-4888-ae54-bcf303ef7101" />


Le shell se termine proprement avec la commande `exit` ou avec `Ctrl+D`.

###  Difficultés rencontrées
- Comprendre que `read()` retourne `≤ 0` lors d'un EOF (End Of File)
- Préférer `strncmp()` à `strcmp()` pour limiter la comparaison et éviter les débordements

---

## Question 4 : Affichage du code de retour

###  Objectif
Afficher dans le prompt le code de retour de la commande précédente :
- `[exit:X]` si terminaison normale (où X est le code de sortie 0-255)
- `[sign:Y]` si terminaison par signal (où Y est le numéro du signal)

### Mise en œuvre
Voir le fichier [`TP4.C`](TP4.c)

#### Concepts utilisés : Macros de `sys/wait.h`

- **`WIFEXITED(status)`** : Retourne vrai si terminaison normale (via `exit()` ou `return`)
- **`WEXITSTATUS(status)`** : Extrait le code de sortie (0-255)
- **`WIFSIGNALED(status)`** : Retourne vrai si terminé par un signal
- **`WTERMSIG(status)`** : Extrait le numéro du signal

#### Principe
Après chaque `wait(&status)`, on analyse `status` avec ces macros pour construire dynamiquement le prompt avec `snprintf()`.

###  Résultat

<img width="1723" height="270" alt="image" src="https://github.com/user-attachments/assets/2cbe4be9-db40-40b4-820a-f14a0f96ea85" />

## Question 5 : Affichage du temps d'execution

###  Objectif
Afficher dans le prompt le  temps d'execution de la commande

### Mise en œuvre
Voir le fichier [`TP5.C`](TP5.c)

#### Concepts utilisés : structure timespec

- indication du temps de départ avec clock_gettime( CLOCK_REALTIME, &start );
- indication du temps de fin avec clock_gettime( CLOCK_REALTIME, &stop );


#### Principe
le temps d'execution est obtenu par stop.tv_sec - start.tv_sec + ( stop.tv_nsec - start.tv_nsec ) / BILLION; (on a le temps en secondes et le nombre de nanosecondes par secondes
Il est important d'indiquer que les valeurs sont des floats pour obtenir le temps en nanosecondes.

###  Résultat

<img width="1723" height="270" alt="image" src="https://github.com/FloretteTsamo/TP-de-Synth-se-Ensea-in-the-Shell/blob/main/TP5.png?raw=true" />

---

## Question 6 : execution d'une commande complexe

###  Objectif
permettre l'execution de commandes avec plusieurs arguments


### Mise en œuvre
Voir le fichier [`Question6.C`](TP6.c)

#### Principe
On veut mettre les arguments dans une liste grâce aux espaces les séparant pour ensuite pouvoir les executer avec execvp.

### Concepts utilisés
Arguments de la ligne de commande : Les arguments sont passés à main(int argc, char **argv) où :

argc : nombre d'arguments
argv : tableau de chaînes, une par argument
argv[0] : nom de la commande

# Exemple pour ls -l *.c :

argc = 3
argv[0] = "ls"
argv[1] = "-l"
argv[2] = "*.c"

execvp() : La fonction prend un tableau d'arguments qui doit obligatoirement se terminer par NULL.
#Fonction de parsing
Nous utilisons strtok() pour découper la chaîne saisie en tokens séparés par des espaces, tabulations ou retours à la ligne :
```
#define MAX_ARGS 64
int parse_command(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \t\n");
    
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;  // Crucial : terminer par NULL
    return i;
}
```

# Intégration
Dans le processus fils, après le fork(), on parse la commande puis on l'exécute :
```
if (pid == 0) {
    char *args[MAX_ARGS];
    parse_command(buffer, args);
    
    if (args[0] != NULL) {
        execvp(args[0], args);
    }
    exit(EXIT_FAILURE);
}
 ```
### RESULTAT

<img width="1080" height="426" alt="image" src="https://github.com/user-attachments/assets/d05967f8-2789-4834-8c9f-2920dfd4a21c" />

### Diffilcultés rencontrées 

strtok() modifie la chaîne : Elle remplace les délimiteurs par \0, ce qui est acceptable ici car on n'a plus besoin de la chaîne originale
Le dernier élément doit être NULL :
Oublier ce NULL final provoque un comportement indéfini dans execvp()
Gestion des chaînes vides : Vérifier que args[0] != NULL avant d'appeler execvp()
---

### Question 7 : Gestion des redirections

### Objectif
Gérer les redirections d'entrée/sortie avec < et >.

###  Mise en œuvre
Voir le fichier [`question7.c`](question7.c)

### Concepts utilisés
**`open()`** : Ouvre un fichier et retourne un descripteur de fichier
``` int open(const char *pathname, int flags, mode_t mode);```

F#### lags principaux :

**`O_RDONLY`**: lecture seule (pour <)
**`O_WRONLY`** : écriture seule (pour >)
**`O_CREAT`** : créer si n'existe pas
**`O_TRUNC`** : effacer le contenu existant

Mode (permissions) :` 0644 = rw-r--r--`
**`dup2()`** : Duplique un descripteur de fichier
```int dup2(int oldfd, int newfd);```
### Principe 
Après `dup2(fd, STDOUT_FILENO)`, tout ce qui est écrit sur stdout va dans le fichier fd.
Descripteurs standards :

**`STDIN_FILENO (0)`*** : entrée standard
**` STDOUT_FILENO (1) `**: sortie standard
**` STDERR_FILENO (2)`** : sortie d'erreur

### Résultats

<img width="592" height="218" alt="6" src="https://github.com/user-attachments/assets/04e7dfb5-3136-4bf2-abfc-86a3bc064a78" />


### schéma explicatif

```
AVANT dup2(fd, STDOUT_FILENO) :
┌─────────────────┐
│ fd 0 → stdin    │
│ fd 1 → stdout   │ (écran)
│ fd 2 → stderr   │
│ fd 3 → file.txt │
└─────────────────┘

APRÈS dup2(fd, STDOUT_FILENO) :
┌─────────────────┐
│ fd 0 → stdin    │
│ fd 1 → file.txt │ ← REDIRIGÉ
│ fd 2 → stderr   │
│ fd 3 → file.txt │
└─────────────────┘
```


### Difficultés rencontrées

- Ordre d'exécution : **`dup2()`** doit être appelé AVANT **`execvp()`** , sinon la redirection ne fonctionne pas
- Fermeture du descripteur : Après **`dup2()`**, il faut fermer le descripteur original avec **`close(fd)`**
- Retrait de > ou < : Mettre **`args[i] = NULL`** pour que **`execvp()`** ne voie pas ces opérateurs
- Gestion d'erreurs : Bien vérifier que **`open()`** ne retourne pas -1 .


---
# Question 8 : Gestion des pipes

### Objectif
Permettre la communication entre deux commandes via le pipe ` | `.

### Mise en œuvre
Voir le fichier [`question8.c`](question8.c)

### Concepts utilisés
- **`pipe()`** : Crée un tube de communication
  ```cint pipe(int pipefd[2]);```

- **`pipefd[0]`** : descripteur de lecture
- **`pipefd[1]`** : descripteur d'écriture

Ce qui est écrit dans pipefd[1] peut être lu depuis pipefd[0]
Buffer FIFO (First In, First Out)

```
Architecture du pipe
Commande 1 (ls)              Commande 2 (wc)
    │                              │
    stdout ──→ pipefd[1]           │
               (écriture)          │
                   │               │
              [BUFFER PIPE]        │
                   │               │
               pipefd[0] ──→ stdin
               (lecture)
```


### Résultat

<img width="1532" height="442" alt="8" src="https://github.com/user-attachments/assets/5d8021e0-0e2d-46d6-8daa-3a2e2c3ec088" />


### Schéma du flux de données
```
Processus père (enseash)
    │
    ├──── fork() ──→ Fils 1 (ls)
    │                   │
    │                   └─ stdout → pipefd[1]
    │                                  │
    │                            [BUFFER PIPE]
    │                                  │
    └──── fork() ──→ Fils 2 (wc)       │
                        │               │
                        └─ stdin ← pipefd[0]
```

### Difficultés rencontrées

- **Fermeture des descripteurs** : Nécessaire pour éviter les blocages

- **Fils 1** : fermer **`pipefd[0]`** (lecture inutile)
- **Fils 2** : fermer **`pipefd[1]`** (écriture inutile)
- **Père** : fermer **`pipefd[0]`** ET **`pipefd[1]`**


- **Ordre des opérations** : Créer le pipe, puis fork les deux fils, puis fermer dans le père
- **Deux wait** : Ne pas oublier d'attendre les deux processus fils avec **`waitpid()`**
- **Deadlock** : Si on oublie de fermer les descripteurs, le programme se bloque indéfiniment


##  Compilation et Installation

### Prérequis
- GCC (GNU Compiler Collection)
- Make
- Linux/Unix



---



### Compilation


```bash
# Cloner le dépôt
git clone https://github.com/florettetsamo/TP-de-Synth-se-Ensea-in-the-Shell.git
cd TP-de-Synth-se-Ensea-in-the-Shell
ou renomez en enseash
# Compiler avec Make
make

# Ou directement avec gcc pour une question spécifique
gcc -Wall -Wextra -std=c99 -o enseash question5.c
```

### Exécution
```bash
./enseash
```

### Nettoyage
```bash
make clean
```

---


##  Références


- Cours de Programmation Système - C. BARÈS, N. PAPAZOGLOU (ENSEA 2024)
- Man pages Linux :``` man 2 fork, man 2 execvp, man 2 wait, man 2 pipe, man 2 dup2, man 2 open```
---

##  Structure du projet

```
enseash/
├── question1.c , TP1.C    # Q1 : Message d'accueil
├── question2.c , TP2.C    # Q2 : Exécution de commandes
├── question3.c , TP3.C    # Q3 : Gestion de exit et Ctrl+D
├── question4.c , TP4.C    # Q4 : Affichage des codes retour
├── question5.c , TP5.C    # Q5 : Affichage des temps d'execution
├── question6.c , TP6.C    # Q6 : Exécution de commandes complexes
├── question7.C            # Q7 : Redirections
├── question8.C            # Q8 : Pipes
├── Makefile           # Compilation
└── README.md          # Ce rapport

```
---
Bilan final

Majeure parties des fonctionnalités implémentées

- Q1 : Message d'accueil
- Q2 : Exécution de commandes simples
- Q3 : Sortie avec exit et Ctrl+D
- Q4 : Codes de retour et signaux
- Q5 : Mesure du temps d'exécution
- Q6 : Arguments multiples
- Q7 : Redirections < et >
- Q8 : Pipes |
- Q9 : Processus en arrière-plan & on pas eu le temps de le finir.

### Évolution du projet

- Séance 1 (Q1-Q4) : Base du shell avec gestion des processus

- Séance 2 (Q5-Q8) : Fonctionnalités avancées (parsing, IPC, background)

### Compétences acquises

- Appels système : fork(), exec(), wait(), pipe(), dup2()

- Gestion des processus et synchronisation

- Communication inter-processus (IPC)

- Parsing de commandes

G- estion des descripteurs de fichiers

---



**Date de dernière mise à jour :16 Décembre 2024  
**Statut :** Séance 1 terminée - Questions 1 à 4
             Séance 2 terminée - correction de la question 4, Question 5, question 6, 7 et 8 
