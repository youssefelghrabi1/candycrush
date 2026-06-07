#ifndef BIBLIO_H_INCLUDED
#define BIBLIO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define LIGNES 12
#define COLONNES 20

// Structure pour le joueur et sa progression
typedef struct {
    char nom[50];
    int niveau_actuel;
    int surcharge; // De 0 à 5 (Game over à 5)
} Joueur;

// Structure pour les objectifs d'un niveau
typedef struct {
    int max_coups;
    int temps_max; // en secondes
    int req_R; // Thermique
    int req_B; // Cryogénique
    int req_G; // Plasma
    int req_Y; // Photonique
    int req_V; // Gravitationnelle
} Objectifs;

// Prototypes des fonctions
void gotoligcol(int lig, int col);
void Color(int couleurDuTexte, int couleurDeFond);
void afficherMenuPrincipal();
void reglesDuJeu();
void afficherCredits();

void initialiserJoueur(Joueur *j);
void chargerSauvegarde(Joueur *j);
void enregistrerSauvegarde(Joueur j);
void configurerNiveau(int niveau, Objectifs *obj);

void genererGrille(char grille[LIGNES][COLONNES]);
void afficherJeu(char grille[LIGNES][COLONNES], Joueur j, Objectifs obj, int coups_joues, int cx, int cy, int sx, int sy, int score_R, int score_B, int score_G, int score_Y, int score_V, int temps_restant);

int detecterEtMarquer(char grille[LIGNES][COLONNES], int marque[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V);
void appliquerGraviteEtRemplissage(char grille[LIGNES][COLONNES]);
void stabiliserReacteur(char grille[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V, Joueur j, Objectifs obj, int coups, int cx, int cy, int temps);

int verifierCoupPossible(char grille[LIGNES][COLONNES]);
void jouerNiveau(Joueur *j);

#endif // BIBLIO_H_INCLUDED