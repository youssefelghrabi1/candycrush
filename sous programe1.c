#include "biblio.h"

void gotoligcol(int lig, int col) {
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
}

void Color(int couleurDuTexte, int couleurDeFond) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
}

void afficherMenuPrincipal() {
    Color(11, 0); // Cyan
    printf("==================================================\n");
    printf("               REACTRON SYSTEM v2026              \n");
    printf("==================================================\n");
    Color(15, 0);
    printf("  1. Lancer la stabilisation du reacteur (Jouer)\n");
    printf("  2. Regles de securite (Regles du jeu)\n");
    printf("  3. Credits techniques\n");
    printf("  4. Fermer le systeme (Quitter)\n");
}

void reglesDuJeu() {
    system("cls");
    Color(14, 0);
    printf("=== REGLES DE STABILISATION DU REACTRON ===\n\n");
    Color(15, 0);
    printf("- Matrice de 12x20 representant le coeur quantique.\n");
    printf("- Deplacez le curseur avec Z, Q, S, D ou les Fleches.\n");
    printf("- Selectionnez avec ESPACE, puis permutez avec une case adjacente.\n");
    printf("- Un coup doit obligatoirement creer un alignement de 3 elements minimum.\n");
    printf("- Un alignement de 5 elements absorbe TOUTES les cellules de ce type !\n");
    printf("- Si vous echouez a un niveau, la Surcharge augmente de 1.\n");
    printf("- A 5 de Surcharge, c'est le EXPLOSION du reacteur (Game Over).\n\n");
    system("pause");
}

void afficherCredits() {
    system("cls");
    Color(13, 0);
    printf("=== CREDITS DU PROJET ===\n\n");
    Color(15, 0);
    printf("Developpe pour l'ECE Paris - Annee 2026\n");
    printf("Algorithmique et Programmation Structuree en C\n");
    printf("Sujet original : Reactron\n\n");
    system("pause");
}

void afficherGrille(char grille[LIGNES][COLONNES], int cx, int cy, int sx, int sy) {
    gotoligcol(3, 0);
    printf("    ");
    for (int j = 0; j < COLONNES; j++) {
        printf("%2d ", j + 1);
    }
    printf("\n   +");
    for (int j = 0; j < COLONNES; j++) printf("---");
    printf("+\n");

    for (int i = 0; i < LIGNES; i++) {
        printf("%2d | ", i + 1);
        for (int j = 0; j < COLONNES; j++) {
            // Gestion de la couleur de fond si selectionne ou sur le curseur
            int bg = 0;
            if (i == cx && j == cy) bg = 8;       // Fond gris pour le curseur
            if (i == sx && j == sy && sx != -1) bg = 4; // Fond rouge pour la selection

            switch (grille[i][j]) {
                case 'R': Color(12, bg); break; // Rouge
                case 'B': Color(9, bg);  break; // Bleu
                case 'G': Color(10, bg); break; // Vert
                case 'Y': Color(14, bg); break; // Jaune
                case 'V': Color(13, bg); break; // Violet
                default:  Color(15, bg); break;
            }
            printf(" %c ", grille[i][j]);
            Color(15, 0);
        }
        printf("|\n");
    }
    printf("   +");
    for (int j = 0; j < COLONNES; j++) printf("---");
    printf("+\n");
}

void afficherJeu(char grille[LIGNES][COLONNES], Joueur j, Objectifs obj, int coups_joues, int cx, int cy, int sx, int sy, int score_R, int score_B, int score_G, int score_Y, int score_V, int temps_restant) {
    // Utilisation de gotoligcol pour rafraîchir sans clignotement
    gotoligcol(0, 0);
    Color(11, 0);
    printf("OPERATEUR : %-15s | NIVEAU : %d | SURCHARGE : %d/5\n", j.nom, j.niveau_actuel, j.surcharge);
    Color(14, 0);
    printf("COUPS : %d/%d | TEMPS RESTANT : %ds   \n", coups_joues, obj.max_coups, temps_restant);
    Color(15, 0);

    afficherGrille(grille, cx, cy, sx, sy);

    printf("\n=== OBJECTIFS ET ENERGIES ABSORBEES ===\n");
    printf("Thermique (R)      : %d / %d  \n", score_R, obj.req_R);
    printf("Cryogenique (B)    : %d / %d  \n", score_B, obj.req_B);
    printf("Plasma (G)         : %d / %d  \n", score_G, obj.req_G);
    printf("Photonique (Y)     : %d / %d  \n", score_Y, obj.req_Y);
    printf("Gravitationnelle(V): %d / %d  \n", score_V, obj.req_V);
    printf("\n(Z,Q,S,D / Fleches : Bouger | ESPACE : Selectionner/Permuter)\n");
}