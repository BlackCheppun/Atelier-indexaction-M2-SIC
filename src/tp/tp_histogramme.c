/* ============================================================================
 *  tp_histogramme.c  --  TP : Test des algorithmes d'histogramme
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "caracteristiques.h"

int main(void) {
    long nrh, nrl, nch, ncl;
    byte **I;
    double hist_norm[NB_NIVEAUX];
    long hist[NB_NIVEAUX];

    /* Variables pour l'image couleur */
    long nrh_c, nrl_c, nch_c, ncl_c;
    rgb8 **I_rgb;
    long hr[NB_NIVEAUX], hg[NB_NIVEAUX], hb[NB_NIVEAUX];

    printf("=== TP Histogramme ===\n\n");

    /* 1. Lecture de l'image (en niveaux de gris) */
    printf("Lecture de l'image images/archive10pgm/arbre1.pgm...\n");
    I = LoadPGM_bmatrix("images/archive10pgm/arbre1.pgm", &nrl, &nrh, &ncl, &nch);

    if (I == NULL) {
        printf("Erreur: impossible de lire l'image images/archive10pgm/arbre1.pgm\n");
        return 1;
    }

    printf("Image chargee: %ld x %ld pixels\n", nrh - nrl + 1, nch - ncl + 1);
    long nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);

    /* 2. Calcul de l'histogramme brut */
    printf("Calcul de l'histogramme brut...\n");
    histogramme_gris(I, nrl, nrh, ncl, nch, hist);

    /* Affichage de quelques valeurs de l'histogramme brut pour vérifier */
    printf("\nAperçu de l'histogramme brut (affichage tous les 32 niveaux) :\n");
    for (int i = 0; i < NB_NIVEAUX; i += 32) {
        printf("Niveau %3d : %ld pixels\n", i, hist[i]);
    }

    /* 3. Normalisation */
    printf("Normalisation de l'histogramme...\n");
    normaliser_histogramme(hist, nb_pixels, hist_norm);

    /* Affichage de l'histogramme normalise */
    printf("\nAperçu de l'histogramme normalise (affichage tous les 32 niveaux) :\n");
    for (int i = 0; i < NB_NIVEAUX; i += 32) {
        printf("Niveau %3d : %f\n", i, hist_norm[i]);
    }

    /* Liberation de la memoire (Gris) */
    free_bmatrix(I, nrl, nrh, ncl, nch);

    /* =========================================================
     * PARTIE COULEUR
     * ========================================================= */
    printf("\n=== Histogramme RGB ===\n");
    printf("Lecture de l'image couleur images/archive10ppm/arbre1.ppm...\n");
    I_rgb = LoadPPM_rgb8matrix("images/archive10ppm/arbre1.ppm", &nrl_c, &nrh_c, &ncl_c, &nch_c);

    if (I_rgb == NULL) {
        printf("Erreur: impossible de lire l'image images/archive10ppm/arbre1.ppm\n");
        return 1;
    }
    
    printf("Image couleur chargee: %ld x %ld pixels\n", nrh_c - nrl_c + 1, nch_c - ncl_c + 1);

    printf("Calcul des histogrammes RGB bruts...\n");
    histogramme_rgb(I_rgb, nrl_c, nrh_c, ncl_c, nch_c, hr, hg, hb);

    printf("\nAperçu des histogrammes bruts RGB (tous les 32 niveaux) :\n");
    for (int i = 0; i < NB_NIVEAUX; i += 32) {
        printf("Niveau %3d : R=%6ld  V=%6ld  B=%6ld\n", i, hr[i], hg[i], hb[i]);
    }

    free_rgb8matrix(I_rgb, nrl_c, nrh_c, ncl_c, nch_c);

    printf("\nTermine avec succes!\n");

    return 0;
}
