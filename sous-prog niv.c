#include "biblio.h"

void configurerNiveau(int niveau, Objectifs *obj) {
    obj->max_coups = 25 - (niveau * 2);
    if (obj->max_coups < 12) obj->max_coups = 12;

    obj->temps_max = 200 - (niveau * 15);
    if (obj->temps_max < 60) obj->temps_max = 60;

    obj->req_R = 10 + (niveau * 4);
    obj->req_B = 10 + (niveau * 4);
    obj->req_G = (niveau >= 2) ? 12 + (niveau * 3) : 0;
    obj->req_Y = (niveau >= 3) ? 12 + (niveau * 3) : 0;
    obj->req_V = (niveau >= 4) ? 15 : 0;
}

void chargerSauvegarde(Joueur *j) {
    FILE *f = fopen("score.txt", "r");
    if (f != NULL) {
        char nom_lu[50];
        int niv, surch;
        while (fscanf(f, "%s %d %d\n", nom_lu, &niv, &surch) != EOF) {
            if (strcmp(nom_lu, j->nom) == 0) {
                j->niveau_actuel = niv;
                j->surcharge = surch;
                printf("\n[PROFIL TROUVE] Reprise au niveau %d (Surcharge: %d/5)\n", j->niveau_actuel, j->surcharge);
                system("pause");
                fclose(f);
                return;
            }
        }
        fclose(f);
    }
    printf("\n[NOUVEAU PROFIL] Creation des acces pour %s.\n", j->nom);
    system("pause");
}

void enregistrerSauvegarde(Joueur j) {
    FILE *f = fopen("score.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s %d %d\n", j.nom, j.niveau_actuel, j.surcharge);
        fclose(f);
    }
}

char randomCellule() {
    char types[] = {'R', 'B', 'G', 'Y', 'V'};
    return types[rand() % 5];
}

void genererGrille(char grille[LIGNES][COLONNES]) {
    // 1. Remplissage purement aléatoire de base
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            grille[i][j] = randomCellule();
        }
    }

    // 2. Nettoyage simple des triplets initiaux pour ne pas bloquer au démarrage
    int marque[LIGNES][COLONNES] = {0};
    int dummy = 0;
    int securite = 0;

    // On boucle max 10 fois pour casser les alignements de départ, pas plus !
    while (detecterEtMarquer(grille, marque, &dummy, &dummy, &dummy, &dummy, &dummy) && securite < 10) {
        for (int i = 0; i < LIGNES; i++) {
            for (int j = 0; j < COLONNES; j++) {
                if (marque[i][j]) {
                    grille[i][j] = randomCellule();
                }
            }
        }
        memset(marque, 0, sizeof(marque));
        securite++;
    }
}