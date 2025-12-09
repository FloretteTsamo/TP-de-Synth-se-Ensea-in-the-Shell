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
Voir le fichier [`question3.c`](TP3.c)

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
Voir le fichier [`question4.c`](TP4.c)

#### Concepts utilisés : Macros de `sys/wait.h`

- **`WIFEXITED(status)`** : Retourne vrai si terminaison normale (via `exit()` ou `return`)
- **`WEXITSTATUS(status)`** : Extrait le code de sortie (0-255)
- **`WIFSIGNALED(status)`** : Retourne vrai si terminé par un signal
- **`WTERMSIG(status)`** : Extrait le numéro du signal

#### Principe
Après chaque `wait(&status)`, on analyse `status` avec ces macros pour construire dynamiquement le prompt avec `snprintf()`.

###  Résultat

<img width="1723" height="270" alt="image" src="https://github.com/user-attachments/assets/2cbe4be9-db40-40b4-820a-f14a0f96ea85" />



###  Difficultés rencontrées
- **Utilisation de `snprintf()`** : Nécessaire pour formater le prompt malgré l'interdiction de `printf()` 
- **Distinction exit/signal** : Bien comprendre que `WIFEXITED` et `WIFSIGNALED` sont mutuellement exclusifs
- **Taille du buffer** : Le prompt étant plus long, il faut augmenter la taille du buffer (`PROMPT_BUFFER_SIZE`)

---

##  Compilation et Installation

### Prérequis
- GCC (GNU Compiler Collection)
- Make
- Linux/Unix

### Compilation

```bash
# Cloner le dépôt
git clone https://github.com/florettetsamo/enseash.git
cd enseash

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

##  Bilan et perspectives

### Ce qui fonctionne (Questions 1-4)
- Affichage du message d'accueil et du prompt
- Lecture et exécution de commandes simples
- Gestion de la sortie (`exit` et `Ctrl+D`)
- Affichage du code de retour et des signaux


###  Limitations actuelles
-  Pas de support des arguments (ex: `ls -la`)
-  Pas de gestion des redirections (`<`, `>`)
-  Pas de support des pipes (`|`)
-  Pas de gestion des processus en arrière-plan (`&`)

###  Prochaines étapes (Questions 6-9)
Les prochaines séances permettront d'ajouter :
- **question 5** : Mesure du temps d'exécution
- **Question 6** : Parsing des arguments de commandes (`ls -la`, `echo hello world`)
- **Question 7** : Redirections d'entrées/sorties (`ls > file.txt`, `cat < input.txt`)
- **Question 8** : Communication par pipes (`ls | wc -l`)
- **Question 9** : Exécution en arrière-plan avec `&` (`sleep 10 &`)

---

##  Références


- Cours de Programmation Système - C. BARÈS, N. PAPAZOGLOU (ENSEA 2024)

---

##  Structure du projet

```
enseash/
├── question1.c         # Q1 : Message d'accueil
├── question2.c         # Q2 : Exécution de commandes
├── question3.c         # Q3 : Gestion de exit et Ctrl+D
├── question4.c         # Q4 : Affichage des codes retour
├── 
├── Makefile           # Compilation
└── README.md          # Ce rapport
```

---

##  Répartition du travail

**Tom PICQUART** : Questions   
**TSAMO Florette Miroslava** : Questions   
**Travail commun** : Débogage, tests et rédaction du rapport

---

**Date de dernière mise à jour :9 Décembre 2024  
**Statut :** Séance 1 terminée - Questions 1 à 4
