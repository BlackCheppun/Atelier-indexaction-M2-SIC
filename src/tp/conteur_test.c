/* ============================================================================
 *  conteur_test.c  --  TP : Test de la detection de contours et de la densite
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "caracteristiques.h"

int main(void) {
    long nrh, nrl, nch, ncl;
    byte **I;
    int **Ix, **Iy, **norme;
    byte **contours;
    byte **norme_affichable;

    printf("Detection et Densite de Contours\n");

    /* 1. Lecture de l'image (en niveaux de gris) */
    // I = LoadPGM_bmatrix("images/archive10pgm/arbre3.pgm", &nrl, &nrh, &ncl, &nch);
    I = LoadPGM_bmatrix("images/image_rond.pgm", &nrl, &nrh, &ncl, &nch);

    if (I == NULL) {
        printf("Erreur: impossible de lire l'image images/archive10pgm/arbre1.pgm\n");
        return 1;
    }

    long nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);

    /* 2. Calcul des gradients horizontaux et verticaux (Sobel) */
    Ix = gradient_x(I, nrl, nrh, ncl, nch);
    Iy = gradient_y(I, nrl, nrh, ncl, nch);

    /* 3. Calcul de la norme du gradient */
    norme = norme_gradient(Ix, Iy, nrl, nrh, ncl, nch);

    /* On convertit la norme en image affichable pour pouvoir la regarder plus tard */
    norme_affichable = imatrix_vers_bmatrix(norme, nrl, nrh, ncl, nch);
    SavePGM_bmatrix(norme_affichable, nrl, nrh, ncl, nch, "resultats/image_rond.pgm");

    /* 4. Seuillage pour obtenir l'image binaire des contours purs */
    /* On utilise un seuil empirique (par exemple 100) */
    int seuil = 100;
    contours = seuillage_imatrix(norme, seuil, nrl, nrh, ncl, nch);
    SavePGM_bmatrix(contours, nrl, nrh, ncl, nch, "resultats/image_rond_contours.pgm");

    /* 5. Calcul de la densite du contour (taux de contour) */
    printf("\nDENSITE DE CONTOUR : \n");
    long nb_pixels_contour = compter_pixels_contour(contours, nrl, nrh, ncl, nch);
    double densite = (double)nb_pixels_contour / (double)nb_pixels;

    printf("Nombre de pixels de contour : %ld\n", nb_pixels_contour);
    printf("Densite (taux) de contour   : %f (soit %.2f%% de l'image)\n", densite, densite * 100.0);

    /* Liberation de la memoire */
    free_bmatrix(I, nrl, nrh, ncl, nch);
    free_imatrix(Ix, nrl, nrh, ncl, nch);
    free_imatrix(Iy, nrl, nrh, ncl, nch);
    free_imatrix(norme, nrl, nrh, ncl, nch);
    free_bmatrix(norme_affichable, nrl, nrh, ncl, nch);
    free_bmatrix(contours, nrl, nrh, ncl, nch);

    printf("\nTermine avec succes!\n");

    return 0;
}
