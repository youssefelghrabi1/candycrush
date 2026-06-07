#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define LIGNES 12
#define COLONNES 20

// Structures du jeu
typedef struct {
    char nom[50];
    int niveau_actuel;
    int surcharge;
} Joueur;

typedef struct {
    int max_coups;
    int temps_max;
    int req_R;
    int req_B;
    int req_G;
    int req_Y;
    int req_V;
} Objectifs;

// Prototypes des fonctions
void gotoligcol(int lig, int col);
void Color(int couleurDuTexte, int couleurDeFond);
void configurerNiveau(int niveau, Objectifs *obj);
void genererGrille(char grille[LIGNES][COLONNES]);
int detecterEtMarquer(char grille[LIGNES][COLONNES], int marque[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V);
void appliquerGraviteEtRemplissage(char grille[LIGNES][COLONNES]);
void stabiliserReacteur(char grille[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V, Joueur j, Objectifs obj, int coups, int cx, int cy, int temps);
void jouerNiveau(Joueur *j);
void enregistrerSauvegarde(Joueur j);

// Déplacement du curseur
void gotoligcol(int lig, int col) {
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
}

// Gestion des couleurs
void Color(int couleurDuTexte, int couleurDeFond) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
}

// Configuration des objectifs par niveau
void configurerNiveau(int niveau, Objectifs *obj) {
    obj->max_coups = 25 - (niveau * 2);
    if (obj->max_coups < 12) obj->max_coups = 12;

    obj->temps_max = 180;
    obj->req_R = 10 + (niveau * 3);
    obj->req_B = 10 + (niveau * 3);
    obj->req_G = (niveau >= 2) ? 10 + (niveau * 2) : 0;
    obj->req_Y = (niveau >= 3) ? 12 + (niveau * 2) : 0;
    obj->req_V = (niveau >= 4) ? 15 : 0;
}

// Sauvegarde basique
void enregistrerSauvegarde(Joueur j) {
    FILE *f = fopen("score.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s %d %d\n", j.nom, j.niveau_actuel, j.surcharge);
        fclose(f);
    }
}

// Générateur de cellule
char randomCellule() {
    char types[] = {'R', 'B', 'G', 'Y', 'V'};
    return types[rand() % 5];
}

// Remplissage initial de la grille
void genererGrille(char grille[LIGNES][COLONNES]) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            grille[i][j] = randomCellule();
        }
    }
}

// Rendu Graphique de la matrice (Requis par le jeu)
void afficherJeu(char grille[LIGNES][COLONNES], Joueur j, Objectifs obj, int coups_joues, int cx, int cy, int sx, int sy, int score_R, int score_B, int score_G, int score_Y, int score_V, int temps_restant) {
    gotoligcol(0, 0);
    Color(11, 0);
    printf("OPERATEUR : %-12s | NIVEAU : %d | SURCHARGE : %d/5  \n", j.nom, j.niveau_actuel, j.surcharge);
    Color(14, 0);
    printf("COUPS : %d/%d | TEMPS RESTANT : %ds      \n", coups_joues, obj.max_coups, temps_restant);
    Color(15, 0);

    printf("\n    ");
    for (int jc = 0; jc < COLONNES; jc++) printf("%2d ", jc + 1);
    printf("\n   +");
    for (int jc = 0; jc < COLONNES; jc++) printf("---");
    printf("+\n");

    for (int i = 0; i < LIGNES; i++) {
        printf("%2d | ", i + 1);
        for (int jc = 0; jc < COLONNES; jc++) {
            int bg = 0;
            if (i == cx && jc == cy) bg = 8;       // Curseur utilisateur (fond gris)
            if (i == sx && jc == sy && sx != -1) bg = 4; // Case sélectionnée (fond rouge)

            switch (grille[i][jc]) {
                case 'R': Color(12, bg); break;
                case 'B': Color(9, bg);  break;
                case 'G': Color(10, bg); break;
                case 'Y': Color(14, bg); break;
                case 'V': Color(13, bg); break;
                default:  Color(15, bg); break;
            }
            printf(" %c ", grille[i][jc]);
            Color(15, 0);
        }
        printf("|\n");
    }
    printf("   +");
    for (int jc = 0; jc < COLONNES; jc++) printf("---");
    printf("+\n");

    printf("\n=== OBJECTIFS ET ENERGIES ABSORBEES ===\n");
    printf("Thermique (R)      : %d / %d    \n", score_R, obj.req_R);
    printf("Cryogenique (B)    : %d / %d    \n", score_B, obj.req_B);
    printf("Plasma (G)         : %d / %d    \n", score_G, obj.req_G);
    printf("Photonique (Y)     : %d / %d    \n", score_Y, obj.req_Y);
    printf("Gravitationnelle(V): %d / %d    \n", score_V, obj.req_V);
    printf("\n[Z,Q,S,D] : Bouger | [ESPACE] : Permuter\n");
}

// Algorithme de détection des alignements de 3 ou plus
int detecterEtMarquer(char grille[LIGNES][COLONNES], int marque[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V) {
    int trouve = 0;
    int detruire_tout_type[256] = {0};

    // Détection Horizontale
    for (int i = 0; i < LIGNES; i++) {
        int j = 0;
        while (j < COLONNES) {
            int len = 1;
            while (j + len < COLONNES && grille[i][j + len] == grille[i][j] && grille[i][j] != ' ') len++;
            if (len >= 3) {
                trouve = 1;
                if (len >= 5) detruire_tout_type[(unsigned char)grille[i][j]] = 1;
                else {
                    for (int k = 0; k < len; k++) marque[i][j + k] = 1;
                }
            }
            j += len;
        }
    }

    // Détection Verticale
    for (int j = 0; j < COLONNES; j++) {
        int i = 0;
        while (i < LIGNES) {
            int len = 1;
            while (i + len < LIGNES && grille[i + len][j] == grille[i][j] && grille[i][j] != ' ') len++;
            if (len >= 3) {
                trouve = 1;
                if (len >= 5) detruire_tout_type[(unsigned char)grille[i][j]] = 1;
                else {
                    for (int k = 0; k < len; k++) marque[i + k][j] = 1;
                }
            }
            i += len;
        }
    }

    // Bonus alignement de 5
    for (int c = 0; c < 256; c++) {
        if (detruire_tout_type[c]) {
            for (int i = 0; i < LIGNES; i++) {
                for (int j = 0; j < COLONNES; j++) {
                    if (grille[i][j] == c) marque[i][j] = 1;
                }
            }
        }
    }

    // Comptage des points cumulés
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (marque[i][j] && grille[i][j] != ' ') {
                switch (grille[i][j]) {
                    case 'R': (*score_R)++; break;
                    case 'B': (*score_B)++; break;
                    case 'G': (*score_G)++; break;
                    case 'Y': (*score_Y)++; break;
                    case 'V': (*score_V)++; break;
                }
            }
        }
    }
    return trouve;
}

// Chute des cases (Gravité)
void appliquerGraviteEtRemplissage(char grille[LIGNES][COLONNES]) {
    for (int j = 0; j < COLONNES; j++) {
        int videIndex = LIGNES - 1;
        for (int i = LIGNES - 1; i >= 0; i--) {
            if (grille[i][j] != ' ') {
                if (i != videIndex) {
                    grille[videIndex][j] = grille[i][j];
                    grille[i][j] = ' ';
                }
                videIndex--;
            }
        }
        for (int i = videIndex; i >= 0; i--) {
            grille[i][j] = randomCellule();
        }
    }
}

// Gestion des combos en chaîne (Cascades) anti-bloquage
void stabiliserReacteur(char grille[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V, Joueur j, Objectifs obj, int coups, int cx, int cy, int temps) {
    int marque[LIGNES][COLONNES];
    int anti_infini = 0;

    while (anti_infini < 8) {
        memset(marque, 0, sizeof(marque));
        if (detecterEtMarquer(grille, marque, score_R, score_B, score_G, score_Y, score_V)) {
            for (int i = 0; i < LIGNES; i++) {
                for (int jc = 0; jc < COLONNES; jc++) {
                    if (marque[i][jc]) grille[i][jc] = ' ';
                }
            }
            afficherJeu(grille, j, obj, coups, cx, cy, -1, -1, *score_R, *score_B, *score_G, *score_Y, *score_V, temps);
            Sleep(250);
            appliquerGraviteEtRemplissage(grille);
            anti_infini++;
        } else {
            break;
        }
    }
}

// Vérification si un coup valide existe sur le plateau actuel
int verifierCoupPossible(char grille[LIGNES][COLONNES]) {
    char temp[LIGNES][COLONNES];
    int dummy[LIGNES][COLONNES] = {0};
    int s = 0;

    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (j + 1 < COLONNES) {
                memcpy(temp, grille, sizeof(temp));
                char c = temp[i][j]; temp[i][j] = temp[i][j+1]; temp[i][j+1] = c;
                if (detecterEtMarquer(temp, dummy, &s,&s,&s,&s,&s)) return 1;
            }
            if (i + 1 < LIGNES) {
                memcpy(temp, grille, sizeof(temp));
                char c = temp[i][j]; temp[i][j] = temp[i+1][j]; temp[i+1][j] = c;
                if (detecterEtMarquer(temp, dummy, &s,&s,&s,&s,&s)) return 1;
            }
        }
    }
    return 0;
}

// Boucle principale d'un niveau de jeu
void jouerNiveau(Joueur *j) {
    Objectifs obj;
    configurerNiveau(j->niveau_actuel, &obj);

    char grille[LIGNES][COLONNES];
    genererGrille(grille);

    int score_R = 0, score_B = 0, score_G = 0, score_Y = 0, score_V = 0;
    int coups_joues = 0;
    int cx = 0, cy = 0;
    int sx = -1, sy = -1;

    time_t debut_temps = time(NULL);
    system("cls");

    // Nettoyage de départ
    int m[LIGNES][COLONNES] = {0};
    int dummy = 0;
    while (detecterEtMarquer(grille, m, &dummy, &dummy, &dummy, &dummy, &dummy)) {
        for (int i = 0; i < LIGNES; i++) {
            for (int jc = 0; jc < COLONNES; jc++) if (m[i][jc]) grille[i][jc] = randomCellule();
        }
        memset(m, 0, sizeof(m));
    }

    while (1) {
        int temps_restant = obj.temps_max - (int)(time(NULL) - debut_temps);

        // Défaite
        if (temps_restant <= 0 || coups_joues >= obj.max_coups) {
            j->surcharge++;
            enregistrerSauvegarde(*j);
            system("cls");
            printf("\n--- ECHEC DU NIVEAU ---\nSurcharge actuelle : %d/5\n", j->surcharge);
            system("pause");
            if (j->surcharge >= 5) {
                printf("\nEXPLOSION DU REACTEUR ! GAME OVER.\n");
                j->surcharge = 0; j->niveau_actuel = 1;
                enregistrerSauvegarde(*j);
                system("pause");
            }
            break;
        }

        // Victoire
        if (score_R >= obj.req_R && score_B >= obj.req_B && score_G >= obj.req_G && score_Y >= obj.req_Y && score_V >= obj.req_V) {
            system("cls");
            printf("\n--- COMPOSANT QUANTIQUE STABILISE ! ---\nPassage au niveau suivant.\n");
            j->niveau_actuel++;
            j->surcharge = 0;
            enregistrerSauvegarde(*j);
            system("pause");
            break;
        }

        // Si aucun coup possible n'existe, on re-génère instantanément
        if (!verifierCoupPossible(grille)) {
            genererGrille(grille);
        }

        afficherJeu(grille, *j, obj, coups_joues, cx, cy, sx, sy, score_R, score_B, score_G, score_Y, score_V, temps_restant);

        // Inputs clavier non-bloquants
        if (kbhit()) {
            int touche = getch();
            if (touche == 0 || touche == 224) {
                touche = getch();
                switch (touche) {
                    case 72: if (cx > 0) cx--; break;
                    case 80: if (cx < LIGNES - 1) cx++; break;
                    case 75: if (cy > 0) cy--; break;
                    case 77: if (cy < COLONNES - 1) cy++; break;
                }
            } else {
                switch (touche) {
                    case 'z': case 'Z': if (cx > 0) cx--; break;
                    case 's': case 'S': if (cx < LIGNES - 1) cx++; break;
                    case 'q': case 'Q': if (cy > 0) cy--; break;
                    case 'd': case 'D': if (cy < COLONNES - 1) cy++; break;
                    case ' ':
                        if (sx == -1) { sx = cx; sy = cy; }
                        else {
                            if ((abs(cx - sx) == 1 && cy == sy) || (abs(cy - sy) == 1 && cx == sx)) {
                                char temp = grille[cx][cy];
                                grille[cx][cy] = grille[sx][sy];
                                grille[sx][sy] = temp;

                                int m_test[LIGNES][COLONNES] = {0};
                                int d = 0;
                                if (detecterEtMarquer(grille, m_test, &d,&d,&d,&d,&d)) {
                                    coups_joues++;
                                    stabiliserReacteur(grille, &score_R, &score_B, &score_G, &score_Y, &score_V, *j, obj, coups_joues, cx, cy, temps_restant);
                                } else {
                                    temp = grille[cx][cy];
                                    grille[cx][cy] = grille[sx][sy];
                                    grille[sx][sy] = temp;
                                }
                            }
                            sx = -1; sy = -1;
                        }
                        break;
                }
            }
        }
        Sleep(60);
    }
}

// Fonction d'entrée (Menu principal)
int main() {
    srand(time(NULL));
    Joueur j = {"Operateur", 1, 0};
    int choix;
    int c;

    do {
        system("cls");
        Color(11, 0);
        printf("==========================================\n");
        printf("          SYSTEME QUANTIQUE REACTRON      \n");
        printf("==========================================\n");
        Color(15, 0);
        printf(" 1. Initialiser le coeur (Jouer)\n");
        printf(" 2. Quitter le systeme\n");
        printf("\nVotre choix : ");

        if (scanf("%d", &choix) != 1) {
            while ((c = getchar()) != '\n' && c != EOF);
            choix = 0;
            continue;
        }

        if (choix == 1) {
            system("cls");
            printf("Identifiant Operateur : ");
            while ((c = getchar()) != '\n' && c != EOF);
            if (scanf("%49s", j.nom) != 1) {
                strcpy(j.nom, "Operateur");
            }
            jouerNiveau(&j);
        }
    }while (choix != 2);

    return 0;
}