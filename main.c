#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>


typedef struct Taquin taquin;
struct Taquin{
    int** board;
    int taille;
    int posZeroColonne;
    int posZeroLigne;
};

void playTaquin();
taquin initBoard();
void libereBoard(int ** board,int taille);
void shuffleBoard(taquin* plateau);
taquin txtToBoard();
taquin personaBoard();
int presentDansTab(int* tab, int taille, int elt);
int nbMoveZeroToPosFinal(taquin plateau);
int estPossible(taquin plateau);
int estGagnant(int** board,int taille);
void afficheBoard(int** board, int taille);
void realisePermu(int indiceA, int indiceB, int plateauEnLigne[]);
int comptePermutation(int plateauEnLigne[], int posZero, int taille);
int trouveIndiceMax(int liste[], int taille);
int moveUp(taquin *plateau);
int moveDown(taquin *plateau);
int moveLeft(taquin *plateau);
int moveRight(taquin *plateau);
int askTaille();
FILE* demandeFile();

int main(){

    initscr(); // Initialiser l'environnement curses
    keypad(stdscr, TRUE); // Activer la prise en charge des touches spéciales

    srand(time(NULL));
    playTaquin();
    endwin();
    return 0;
}






void playTaquin(){

    taquin plateau = personaBoard();
    //taquin plateau= initBoard();
    //taquin plateau = txtToBoard();
    int taille= plateau.taille;
    int gagnant = estGagnant(plateau.board, taille);
    afficheBoard(plateau.board, taille); 
    int touche;

    while(((touche= getch()) != 'q' && (gagnant==0))){  
        switch(touche){
            case KEY_UP:
                moveUp(&plateau);
                break;
            case KEY_DOWN:
                moveDown(&plateau);
                break;       
            case KEY_LEFT:
                moveLeft(&plateau);
                break;
            case KEY_RIGHT:
                moveRight(&plateau);
                break;       
        }
        afficheBoard(plateau.board, taille);
        if((plateau.posZeroColonne==taille-1)&&(plateau.posZeroLigne==taille-1)){
            gagnant=estGagnant(plateau.board, taille);

        };
    }

    if(gagnant==1){
        printw("\nVous avez gagné, appuyé sur une touche pour quitter.\n");
        getch(); 
    }  
    refresh(); 
    
    libereBoard(plateau.board,taille); 
}


/**
 * @brief Vérifie si un élément est présent dans un tableau d'entiers.
 *
 * Parcourt le tableau pour vérifier si un élément donné est présent.
 *
 * @param tab Le tableau d'entiers à vérifier.
 * @param taille La taille du tableau.
 * @param elt L'élément à rechercher dans le tableau.
 *
 * @return 1 si l'élément est présent, 0 sinon.
 */
int presentDansTab(int* tab, int taille, int elt) {
    int estPresent = 0;

    // Parcourt le tableau pour vérifier la présence de l'élément
    for (int i = 0; i < taille; i++) {
        if (tab[i] == elt) {
            estPresent = 1; // L'élément est présent dans le tableau
        }
    }

    return estPresent;
}





/**
 * @brief Initialise un plateau de Taquin en permettant à l'utilisateur de saisir les nombres.
 *
 * La fonction demande à l'utilisateur de saisir les nombres pour initialiser le plateau de Taquin.
 * Elle garantit que chaque nombre de 0 à (taille * taille - 1) est inclus une seule fois.
 *
 * @param taille La taille du plateau de Taquin.
 *
 * @return Un objet de type `taquin` représentant le plateau de Taquin initialisé par l'utilisateur.
 */
taquin personaBoard() {
    taquin plateau;
    plateau.taille = askTaille();
    int taille = plateau.taille;
    plateau.board = malloc(taille * sizeof(int*));

    // Alloue dynamiquement de la mémoire pour chaque ligne du plateau
    for (int i = 0; i < taille; i++) {
        plateau.board[i] = malloc(taille * sizeof(int));
    }

    int* tabDejaDis = malloc((taille * taille) * sizeof(int));
    int k = 0; // Compteur pour suivre le nombre d'éléments déjà saisis
    int valid = 0; // Variable pour vérifier la validité de l'entrée utilisateur
    int temp; // Variable temporaire pour stocker la saisie utilisateur

    // Boucle pour remplir le plateau avec les nombres saisis par l'utilisateur
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            clear(); // Efface l'écran pour une saisie plus claire
            valid = 0; // Réinitialise la validité à faux

            // Boucle pour demander à l'utilisateur de saisir un nombre valide
            while (valid == 0) {
                scanw("%d ", &temp); // Saisie utilisateur

                // Vérifie si la saisie est dans la plage valide
                if ((temp >= 0) && (temp <= ((taille * taille) - 1))) {
                    // Vérifie si le nombre n'a pas encore été saisi
                    if (presentDansTab(tabDejaDis, k, temp) == 0) {
                        valid = 1; // La saisie est valide
                        plateau.board[i][j] = temp; // Enregistre le nombre dans le plateau
                    } else {
                        printw("Nombre déjà saisi\n"); // Affiche un message si le nombre est déjà saisi
                    }
                } else {
                    printw("Entrée non valide\n"); // Affiche un message si la saisie n'est pas valide
                }
                refresh(); // Rafraîchit l'écran pour afficher les messages
            }

            tabDejaDis[k] = plateau.board[i][j]; // Enregistre le nombre dans le tableau des nombres déjà saisis
            k++;

            if (plateau.board[i][j] == 0) {
                plateau.posZeroColonne = j; // Enregistre la colonne de la case vide
                plateau.posZeroLigne = i;   // Enregistre la ligne de la case vide
            }
        }
    }

    free(tabDejaDis);  // Libération de la mémoire allouée dynamiquement pour le tableau tabDejaDis
    tabDejaDis = NULL; // Éviter les accès indésirables à la mémoire libérée.

    return plateau; // Retourne le plateau de Taquin initialisé par l'utilisateur
}






/**
 * @brief Libère la mémoire allouée dynamiquement pour le plateau de jeu.
 *
 * @param board Le tableau représentant le plateau de jeu.
 * @param taille La taille du plateau.
 *
 * @return Aucune valeur de retour.
 */
void libereBoard(int **board, int taille){
    for(int i = 0; i < taille; i++){
        free(board[i]);
        board[i] = NULL;
    }
    free(board);
    board = NULL;
}





/**
 * @brief Initialise un plateau de Taquin en créant un tableau avec des nombres mélangés.
 *
 * @param taille La taille du côté du plateau de Taquin.
 *
 * @return Un objet de type `taquin` représentant un plateau de Taquin initialisé.
 */
taquin initBoard() {
    taquin plateau; // Déclaration d'une variable de type `taquin` pour représenter le plateau de Taquin.
    plateau.taille = askTaille();
    int taille = plateau.taille;
    plateau.posZeroColonne = 0;
    plateau.posZeroLigne = 0;
    
    // Allocation dynamique du tableau 2D pour représenter le plateau.
    plateau.board = malloc(taille * sizeof(int*));

    // Initialisation du tableau 2D avec des nombres consécutifs.
    int compteur = 0; 
    for (int i = 0; i < taille; i++) {
        plateau.board[i] = malloc(taille * sizeof(int));
        for (int j = 0; j < taille; j++) {
            plateau.board[i][j] = compteur;
            compteur += 1;
        }        
    }

    // Mélange aléatoire du tableau à une dimension.
    shuffleBoard(&plateau);

    return plateau; // Retourne le plateau de Taquin initialisé.
}








/**
 * @brief Vérifie si le plateau de Taquin est dans un état gagnant.
 * 
 * @param board Le tableau représentant le plateau de Taquin.
 * @param taille La taille du côté du plateau.
 * 
 * @return 1 si le plateau est gagnant, 0 sinon.
 */
int estGagnant(int** board, int taille) {
    int gagnant = 1; // Initialiser la variable gagnant à vrai (1)
    int i = 0;
    int j = 0;
    int nbTaquinAVerif = 1;

    // Parcourir les lignes du plateau
    while ((gagnant != 0) && (i < taille)) {
        j = 0;

        // Parcourir les colonnes du plateau
        while ((gagnant != 0) && (j < taille)) {
            // Vérifier si le taquin actuel n'est pas égal au numéro attendu
            if (board[i][j] != nbTaquinAVerif) {
                gagnant = 0; // Le plateau n'est pas gagnant
            }

            nbTaquinAVerif++; // Passer au numéro suivant
            if (nbTaquinAVerif == taille * taille) {
                nbTaquinAVerif = 0; // Réinitialiser à 0 après la dernière valeur possible
            }
            j++;
        }

        i++;
    }

    return gagnant; // Renvoyer le résultat (1 si gagnant, 0 sinon)
}





/**
 * @brief Calcule le nombre de mouvements nécessaires pour déplacer la case vide
 *        vers sa position finale dans le jeu de Taquin.
 * 
 * @param plateau La structure Taquin représentant l'état actuel du plateau.
 * 
 * @return Le nombre de mouvements nécessaires pour déplacer la case vide
 *         vers sa position finale.
 */
int nbMoveZeroToPosFinal(taquin plateau) {
    // Le nombre de mouvements nécessaires est la somme des distances de la case vide
    // par rapport à sa position finale en termes de lignes et de colonnes.
    return ((plateau.taille - 1 - plateau.posZeroLigne) + (plateau.taille - 1 - plateau.posZeroColonne));
}





/**
 * @brief Trouve l'indice du maximum dans une liste jusqu'à une certaine taille.
 *
 * @param liste La liste dans laquelle rechercher le maximum.
 * @param taille La taille jusqu'à laquelle effectuer la recherche.
 *
 * @return L'indice du maximum dans la liste jusqu'à la taille spécifiée.
 */
int trouveIndiceMax(int liste[], int taille) {
    int max = liste[0];
    int indiceMax = 0;

    // Parcourir la liste jusqu'à la taille spécifiée
    for (int i = 0; i < taille; i++) {
        if (liste[i] > max) {
            max = liste[i];
            indiceMax = i;
        }
    }

    return indiceMax;
}






/**
 * @brief Réalise une permutation entre deux indices d'une liste.
 *
 * @param indiceA Premier indice à permuter.
 * @param indiceB Deuxième indice à permuter.
 * @param plateauEnLigne La liste dans laquelle la permutation est effectuée.
 */
void realisePermu(int indiceA, int indiceB, int plateauEnLigne[]) {
    // Échanger les éléments aux indices A et B dans la liste
    int temp = plateauEnLigne[indiceA];
    plateauEnLigne[indiceA] = plateauEnLigne[indiceB];
    plateauEnLigne[indiceB] = temp;
}





/**
 * @brief Compte le nombre de permutations dans une liste en utilisant
 * un algorithme de tri.
 *
 * @param plateauEnLigne La liste à évaluer.
 * @param posZero Position de la case vide dans la liste.
 * @param taille La taille du côté du plateau.
 *
 * @return Le nombre de permutations nécessaires pour trier la liste.
 */
int comptePermutation(int plateauEnLigne[], int posZero, int taille) {
    int permu = 0;
    int indiceMax;

    // Si la case vide n'est pas à la fin de la liste
    if (posZero != (taille * taille - 1)) {
        // Réaliser une permutation pour déplacer la case vide à la fin
        realisePermu(taille * taille - 1, posZero, plateauEnLigne);
        permu++;
    }

    int tailleListeATrie = taille * taille - 1;
    
    int trie = 0;
    int i = tailleListeATrie - 1;
    
    // Boucle réalisant l'algorithme de tri
    while ((trie == 0) && (i >= 0)) {
        // Trouver l'indice du maximum dans la partie non triée
        indiceMax = trouveIndiceMax(plateauEnLigne, i + 1);

        // Si l'indice du maximum est égal à l'indice courant, la liste est triée
        if (indiceMax == i) {
            trie = 1;
        } else {
            // Réaliser une permutation pour placer le maximum à sa position correcte
            realisePermu(indiceMax, i, plateauEnLigne);
            permu++;
        }
        
        i--;
    }
    return permu; // Renvoyer le nombre total de permutations
}







/**
 * @brief Détermine si un plateau de Taquin est possible à résoudre.
 *
 * La fonction évalue la possibilité de résoudre le Taquin en se basant
 * sur la parité de la permutation des nombres et la parité de la case vide.
 *
 * @param plateau La structure Taquin représentant l'état du plateau.
 *
 * @return 1 si le plateau est possible à résoudre, 0 sinon.
 */
int estPossible(taquin plateau) {
    int taille = plateau.taille;
    int plateauEnLigne[taille * taille]; // Tableau 1D représentant le plateau
    int k = 0;
    int posZero;

    // Convertir le tableau 2D en tableau 1D
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            plateauEnLigne[k] = plateau.board[i][j];

            if (plateau.board[i][j] == 0) {
                posZero = k; // Position de la case vide dans le tableau 1D
            }
            k++;
        }
    }

    // Compte le nombre de permutations
    int inversions = comptePermutation(plateauEnLigne, posZero, taille);

    // Compte le nombre de mouvements que doit faire le zéro pour atteindre la position finale
    int zeroToPosFinal = nbMoveZeroToPosFinal(plateau);

    printw("%d %d\n", zeroToPosFinal, inversions);
    getch();

    // Le problème est soluble si la parité de la permutation est identique
    // à la parité de la case vide
    return (inversions % 2 == zeroToPosFinal % 2);
}





/**
 * @brief Affiche le plateau de jeu du Taquin sous forme graphique.
 *
 * @param board Le tableau représentant le plateau de jeu.
 * @param taille La taille du plateau (nombre de lignes et colonnes).
 */
void afficheBoard(int** board, int taille) {
    clear();
    // Affiche la ligne supérieure du plateau
    for (int j = 0; j < taille; j++) {
        printw("%s", "===");
    }
    printw("\n");

    // Affiche chaque ligne du plateau
    for (int i = 0; i < taille; i++) {
        // Affiche chaque colonne de la ligne actuelle
        for (int j = 0; j < taille; j++) {
            // Vérifie si la case est vide (contient 0)
            if (board[i][j] == 0) {
                printw("|  ");  // Affiche une case vide avec deux espaces
            } else {
                printw("|%2d", board[i][j]);  // Affiche le nombre avec un espacement de 2 caractères
            }

            // Si c'est la dernière colonne, ajoute un trait vertical à la fin de la ligne
            if (j == taille - 1) {
                printw("|\n");
            }
        }

        // Si ce n'est pas la dernière ligne, ajoute une ligne de séparation horizontale
        if (i != taille - 1) {
            for (int j = 0; j < taille; j++) {
                printw("|--");

                // Si c'est la dernière colonne, ajoute un trait vertical à la fin de la ligne
                if (j == taille - 1) {
                    printw("|\n");
                }
            }
        }
        
    }

    // Affiche la ligne inférieure du plateau
    for (int i = 0; i < taille; i++) {
        printw("%s", "===");
    }
    printw("\n");

    refresh();
}







/**
 * @brief Déplace la case vide vers le haut dans le jeu de Taquin.
 *
 * @param plateau Un pointeur vers la structure Taquin représentant le plateau de jeu.
 *
 * @return 1 si le déplacement est effectué avec succès, 0 sinon.
 */
int moveUp(taquin *plateau) {
    // Copie le plateau pour effectuer les modifications
    taquin p = *plateau;

    // Si la case "vide" est en haut du plateau, impossible de monter.
    if (p.posZeroLigne == 0) {
        return 0;
    }

    // Effectue le déplacement vers le haut
    p.board[p.posZeroLigne][p.posZeroColonne] = p.board[p.posZeroLigne - 1][p.posZeroColonne];
    p.posZeroLigne -= 1;
    p.board[p.posZeroLigne][p.posZeroColonne] = 0;

    // Met à jour le plateau d'origine avec les modifications
    *plateau = p;

    // Retourne 1 pour indiquer que le déplacement a réussi
    return 1;
}







/**
 * @brief Déplace la case vide vers le bas dans le jeu de Taquin.
 *
 * @param plateau Un pointeur vers la structure Taquin représentant le plateau de jeu.
 *
 * @return 1 si le déplacement est effectué avec succès, 0 sinon.
 */
int moveDown(taquin *plateau) {
    // Copie le plateau pour effectuer les modifications
    taquin p = *plateau;

    // Si la case "vide" est en bas du plateau, impossible de descendre.
    if (p.posZeroLigne == p.taille - 1) {
        return 0;
    }

    // Effectue le déplacement vers le bas
    p.board[p.posZeroLigne][p.posZeroColonne] = p.board[p.posZeroLigne + 1][p.posZeroColonne];
    p.posZeroLigne += 1;
    p.board[p.posZeroLigne][p.posZeroColonne] = 0;

    // Met à jour le plateau d'origine avec les modifications
    *plateau = p;

    // Retourne 1 pour indiquer que le déplacement a réussi
    return 1;
}







/**
 * @brief Déplace la case vide vers la gauche dans le jeu de Taquin.
 *
 * @param plateau Un pointeur vers la structure Taquin représentant le plateau de jeu.
 *
 * @return 1 si le déplacement est effectué avec succès, 0 sinon.
 */
int moveLeft(taquin *plateau) {
    // Copie le plateau pour effectuer les modifications
    taquin p = *plateau;

    // Si la case "vide" est à gauche du plateau, impossible d'aller plus à gauche.
    if (p.posZeroColonne == 0) {
        return 0;
    }

    // Effectue le déplacement vers la gauche
    p.board[p.posZeroLigne][p.posZeroColonne] = p.board[p.posZeroLigne][p.posZeroColonne - 1];
    p.posZeroColonne -= 1;
    p.board[p.posZeroLigne][p.posZeroColonne] = 0;

    // Met à jour le plateau d'origine avec les modifications
    *plateau = p;

    // Retourne 1 pour indiquer que le déplacement a réussi
    return 1;
}







/**
 * @brief Déplace la case vide vers la droite dans le jeu de Taquin.
 *
 * @param plateau Un pointeur vers la structure Taquin représentant le plateau de jeu.
 *
 * @return 1 si le déplacement est effectué avec succès, 0 sinon.
 */
int moveRight(taquin *plateau) {
    // Copie le plateau pour effectuer les modifications
    taquin p = *plateau;

    // Si la case "vide" est à droite du plateau, impossible d'aller plus à droite.
    if (p.posZeroColonne == p.taille - 1) {
        return 0;
    }

    // Effectue le déplacement vers la droite
    p.board[p.posZeroLigne][p.posZeroColonne] = p.board[p.posZeroLigne][p.posZeroColonne + 1];
    p.posZeroColonne += 1;
    p.board[p.posZeroLigne][p.posZeroColonne] = 0;

    // Met à jour le plateau d'origine avec les modifications
    *plateau = p;

    // Retourne 1 pour indiquer que le déplacement a réussi
    return 1;
}






/**
 * @brief Effectue un mélange aléatoire du plateau de Taquin en effectuant des mouvements.
 *
 * @param plateau Un pointeur vers la structure `taquin` représentant le plateau de Taquin à mélanger.
 *
 * @return Aucune valeur de retour.
 */
void shuffleBoard(taquin* plateau) {
    int taille = (*plateau).taille;

    // Effectue un certain nombre de mouvements aléatoires pour mélanger le plateau.
    for (int i = 0; i < 100 * (taille * taille); i++) {
        int j = rand() % 4;

        // Effectue le mouvement en fonction de la valeur aléatoire.
        switch (j) {
            case 0:
                if (moveUp(plateau) == 0) {
                    moveDown(plateau);
                }
                break;
            case 1:
                if (moveDown(plateau) == 0) {
                    moveUp(plateau);
                }
                break;
            case 2:
                if (moveLeft(plateau) == 0) {
                    moveRight(plateau);
                }
                break;
            case 3:
                if (moveRight(plateau) == 0) {
                    moveLeft(plateau);
                }
                break;
        }
    }
}




/**
 * @brief Demande à l'utilisateur de spécifier le nom d'un fichier et retourne son pointeur.
 *
 * Cette fonction demande à l'utilisateur d'entrer le nom d'un fichier et tente d'ouvrir
 * le fichier en mode lecture. Si l'ouverture du fichier réussit, le pointeur vers le fichier
 * est retourné, sinon un message d'erreur est affiché.
 *
 * @return Le pointeur vers le fichier ouvert.
 */
FILE* demandeFile() {
    char fichier[40];
    int valid = 0;
    FILE* file;

    while (valid == 0) {
        printw("Entrer le nom du fichier : ");
        scanw("%s", fichier);

        file = fopen(fichier, "rt"); // Ouverture du fichier en mode lecture

        // Gestion de la lecture du fichier
        if (file) {
            printw("\nFichier %s ouvert\n", fichier);
            valid = 1;
        } else {
            printw("\nErreur en ouvrant le fichier %s\n", fichier);
        }
        refresh();
    }
    
    return file;
}





/**
 * @brief Convertit les données d'un fichier texte en un plateau de Taquin.
 *
 * Cette fonction lit les données d'un fichier texte spécifié par l'utilisateur
 * et les convertit en une structure de plateau de Taquin. Le fichier doit contenir
 * la taille du plateau suivie des éléments du plateau, avec 0 représentant la case vide.
 *
 * @return Un objet de type `taquin` représentant le plateau de Taquin extrait du fichier.
 */
taquin txtToBoard() {
    taquin plateau;

    FILE* file = demandeFile(); // Demande à l'utilisateur de sélectionner un fichier

    int taille;

    // Lire la taille du plateau depuis le fichier
    fscanf(file, "%d", &taille);
    plateau.taille = taille;

    // Allocation dynamique du tableau 2D pour représenter le plateau.
    plateau.board = malloc(taille * sizeof(int*));

    // Remplissage du plateau à partir des données du fichier
    for (int i = 0; i < taille; i++) {
        plateau.board[i] = malloc(taille * sizeof(int));
        for (int j = 0; j < taille; j++) {
            fscanf(file, " %d", &plateau.board[i][j]);
            if (plateau.board[i][j] == 0) {
                plateau.posZeroLigne = i;
                plateau.posZeroColonne = j;
            }
        }
    }

    // Fermeture du fichier
    fclose(file);

    return plateau;
}


/**
 * @brief Demande à l'utilisateur d'entrer la taille du plateau de Taquin.
 *
 * Cette fonction affiche un message demandant à l'utilisateur d'entrer la taille du plateau
 * de jeu du Taquin. Elle vérifie ensuite si la taille entrée est supérieure à 2. Si ce n'est pas le cas,
 * elle demande à nouveau à l'utilisateur d'entrer une taille valide.
 *
 * @return La taille du plateau de jeu du Taquin saisie par l'utilisateur.
 */
int askTaille() {
    int taille;
    int valid = 0;

    clear(); // Efface l'écran pour une interface utilisateur plus propre

    while (valid == 0) {
        printw("Entrer la taille du plateau voulu : ");
        scanw("%d", &taille);

        // Vérifie si la taille entrée est supérieure à 2
        if (taille > 2) {
            valid = 1;
        } else {
            printw("La taille doit être supérieure à 2.\n");
        }

        refresh(); // Rafraîchit l'affichage pour mettre à jour l'interface utilisateur
    }

    return taille;
}