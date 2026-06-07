#include "biblio.h"

int detecterEtMarquer(char grille[LIGNES][COLONNES], int marque[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V) {
    int alignement_trouve = 0;
    int detruire_tout_type[256] = {0};

    // 1. Detection Horizontale
    for (int i = 0; i < LIGNES; i++) {
        int j = 0;
        while (j < COLONNES) {
            int len = 1;
            while (j + len < COLONNES && grille[i][j + len] == grille[i][j] && grille[i][j] != ' ') {
                len++;
            }
            if (len >= 3) {
                alignement_trouve = 1;
                if (len >= 5) {
                    detruire_tout_type[(unsigned char)grille[i][j]] = 1;
                } else {
                    for (int k = 0; k < len; k++) marque[i][j + k] = 1;
                }
            }
            j += len;
        }
    }

    // 2. Detection Verticale
    for (int j = 0; j < COLONNES; j++) {
        int i = 0;
        while (i < LIGNES) {
            int len = 1;
            while (i + len < LIGNES && grille[i + len][j] == grille[i][j] && grille[i][j] != ' ') {
                len++;
            }
            if (len >= 3) {
                alignement_trouve = 1;
                if (len >= 5) {
                    detruire_tout_type[(unsigned char)grille[i][j]] = 1;
                } else {
                    for (int k = 0; k < len; k++) marque[i + k][j] = 1;
                }
            }
            i += len;
        }
    }

    // Règle spéciale alignement de 5
    for (int c = 0; c < 256; c++) {
        if (detruire_tout_type[c]) {
            for (int i = 0; i < LIGNES; i++) {
                for (int j = 0; j < COLONNES; j++) {
                    if (grille[i][j] == c) marque[i][j] = 1;
                }
            }
        }
    }

    // Comptage des points
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

    return alignement_trouve;
}

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
            grille[i][j] = "RBUGV"[rand() % 5];
        }
    }
}

void stabiliserReacteur(char grille[LIGNES][COLONNES], int *score_R, int *score_B, int *score_G, int *score_Y, int *score_V, Joueur j, Objectifs obj, int coups, int cx, int cy, int temps) {
    int marque[LIGNES][COLONNES];
    int continue_cascade = 1;
    int securite = 0;

    while (continue_cascade && securite < 15) {
        memset(marque, 0, sizeof(marque));
        if (detecterEtMarquer(grille, marque, score_R, score_B, score_G, score_Y, score_V)) {
            for (int i = 0; i < LIGNES; i++) {
                for (int jc = 0; jc < COLONNES; jc++) {
                    if (marque[i][jc]) grille[i][jc] = ' ';
                }
            }
            afficherJeu(grille, j, obj, coups, cx, cy, -1, -1, *score_R, *score_B, *score_G, *score_Y, *score_V, temps);
            Sleep(200);

            appliquerGraviteEtRemplissage(grille);
            afficherJeu(grille, j, obj, coups, cx, cy, -1, -1, *score_R, *score_B, *score_G, *score_Y, *score_V, temps);
            Sleep(150);
            securite++;
        } else {
            continue_cascade = 0;
        }
    }
}

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
    int temps_restant = obj.temps_max;

    system("cls");

    while (1) {
        time_t maintenant = time(NULL);
        int temps_actuel = (int)(maintenant - debut_temps);
        temps_restant = obj.temps_max - temps_actuel;

        // Conditions de défaite
        if (temps_restant <= 0 || coups_joues >= obj.max_coups) {
            j->surcharge++;
            enregistrerSauvegarde(*j);
            system("cls");
            Color(12, 0);
            printf("\n==================================================\n");
            printf("       ECHEC DE STABILISATION - CRITIQUE           \n");
            printf("==================================================\n");
            Color(15, 0);
            printf("Le reacteur s'emballe... Surcharge actuelle : %d/5\n\n", j->surcharge);
            system("pause");

            if (j->surcharge >= 5) {
                system("cls");
                Color(12, 0);
                printf("\n!!! GAME OVER FINAL : EXPLOSION DU REACTEUR !!!\n\n");
                Color(15, 0);
                j->surcharge = 0;
                j->niveau_actuel = 1;
                enregistrerSauvegarde(*j);
                system("pause");
            }
            break;
        }

        // Condition de victoire
        if (score_R >= obj.req_R && score_B >= obj.req_B && (obj.req_G == 0 || score_G >= obj.req_G) && (obj.req_Y == 0 || score_Y >= obj.req_Y) && (obj.req_V == 0 || score_V >= obj.req_V)) {
            system("cls");
            Color(10, 0);
            printf("\n==================================================\n");
            printf("     NIVEAU %d STABILISE AVEC SUCCES !             \n", j->niveau_actuel);
            printf("==================================================\n");
            Color(15, 0);
            j->niveau_actuel++;
            j->surcharge = 0;
            enregistrerSauvegarde(*j);
            system("pause");
            break;
        }

        // Si la grille n'a plus aucun coup possible, on la régénère simplement en un coup sans bloquer
        if (!verifierCoupPossible(grille)) {
            genererGrille(grille);
        }

        afficherJeu(grille, *j, obj, coups_joues, cx, cy, sx, sy, score_R, score_B, score_G, score_Y, score_V, temps_restant);

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
                        if (sx == -1) {
                            sx = cx; sy = cy;
                        } else {
                            if ((abs(cx - sx) == 1 && cy == sy) || (abs(cy - sy) == 1 && cx == sx)) {
                                char c = grille[cx][cy];
                                grille[cx][cy] = grille[sx][sy];
                                grille[sx][sy] = c;

                                int m[LIGNES][COLONNES] = {0};
                                int d = 0;
                                if (detecterEtMarquer(grille, m, &d,&d,&d,&d,&d)) {
                                    coups_joues++;
                                    stabiliserReacteur(grille, &score_R, &score_B, &score_G, &score_Y, &score_V, *j, obj, coups_joues, cx, cy, temps_restant);
                                } else {
                                    c = grille[cx][cy];
                                    grille[cx][cy] = grille[sx][sy];
                                    grille[sx][sy] = c;
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