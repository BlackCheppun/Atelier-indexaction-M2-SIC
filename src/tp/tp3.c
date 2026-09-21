/* ============================================================================
 *  tp3.c  --  TP : transformee de Hough et detection de droites.
 *
 *  Les fonctions generiques (produit_convolution, binarisation) vivent
 *  desormais dans la bibliotheque src/lib/traitement.c.
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"

#include "traitement.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

byte **Transoformer_Hough(byte **image, long nrl, long nrh, long ncl, long nch){
    byte **mat_vote;
    long nb_pixels_diagonale = ((nrh - nrl + 1) < (nch - ncl + 1)) ? (nrh - nrl + 1) : (nch - ncl + 1);
    mat_vote=bmatrix(0, 179, 0, nb_pixels_diagonale);

    for (int i = 0; i <180 ; i++) {
        for (int j = 0; j <= nb_pixels_diagonale ; j++) {
            mat_vote[i][j]=(byte) 0;
        }
    }

    double p;
    for (int i = nrl; i <= nrh ; i++) {
        for (int j = ncl; j <= nch ; j++) {
            if((int)image[i][j] == 255){
                for (int theta=0 ; theta<180 ; theta++){
                    // Conversion de degres en radians
                    double theta_rad = theta * M_PI / 180.0;

                    // Calcul de x*cos(theta) + y*sin(theta)
                    p = i * sin(theta_rad) + j * cos(theta_rad);
                    mat_vote[theta][(int)round(p)] = (mat_vote[theta][(int)round(p)] < 255)? mat_vote[theta][(int)round(p)]+(byte)1 :(byte)255;
                }

            }
        }

    }

    return mat_vote;

}

rgb8 ** detection_ligne (byte **image, long nrl, long nrh, long ncl, long nch, byte ** mat_hough, long rs, long re , long cs, long ce){
    rgb8 **newImage;
    newImage = rgb8matrix(nrl, nrh, ncl, nch);
    for (int i = nrl; i <= nrh ; i++) {
        for (int j = ncl; j <= nch ; j++) {
            newImage[i][j].r=image[i][j];
            newImage[i][j].g=image[i][j];
            newImage[i][j].b=image[i][j];

        }
    }

    int theta_max=0;
    int ro_max=-10000;
    int val_max=0;
    //chercher le point avec le plus de votes
    for (int i = rs; i <= re ; i++) {
        for (int j = cs; j <= ce ; j++) {
            if((int)mat_hough[i][j]>val_max){
                val_max=(int) mat_hough[i][j];
                theta_max=i;
                ro_max=j;
            }
        }
    }

    int y;
    double theta_max_rad = theta_max * M_PI / 180.0;
    for (int x = ncl; x <= nch ;x++) {
        y=round((ro_max-x*cos(theta_max_rad))/sin(theta_max_rad));
        newImage[x][y].r=255;
        newImage[x][y].g=0;
        newImage[x][y].b=0;
    }

    return newImage;
}


int main(void) {
    long nrh, nrl, nch, ncl;
    byte **I;
    byte **I_bin;
    byte **I_Hough;
    rgb8 **I_ligne;

    printf("=== TP3 - Transformee de Hough ===\n\n");

    // Lecture de l'image avec LoadPGM_bmatrix
    printf("Lecture de l'image images/route0.pgm...\n");
    I = LoadPGM_bmatrix("images/route0.pgm", &nrl, &nrh, &ncl, &nch);

    if (I == NULL) {
        printf("Erreur: impossible de lire l'image images/route0.pgm\n");
        return 1;
    }

    printf("Image chargee: %ld x %ld pixels\n", nrh - nrl + 1, nch - ncl + 1);

    // Binarisation (fonction de la bibliotheque src/lib/traitement.c)
    printf("\nApplication de la binarisation...\n");
    I_bin = binarisation(I, 120, nrl, nrh, ncl, nch);

    printf("\nApplication de la transformee de Hough...\n");
    I_Hough = Transoformer_Hough(I_bin, nrl, nrh, ncl, nch);

    printf("\nAffichage des lignes...\n");
    long nb_pixels_diagonale = ((nrh - nrl + 1) < (nch - ncl + 1)) ? (nrh - nrl + 1) : (nch - ncl + 1);
    I_ligne = detection_ligne(I, nrl, nrh, ncl, nch,I_Hough,0,179,0,nb_pixels_diagonale);


    // Sauvegarde des images
    printf("\nSauvegarde des images...\n");
    SavePGM_bmatrix(I_bin, nrl, nrh, ncl, nch, "resultats/route0_bin.pgm");
    SavePGM_bmatrix(I_Hough, 0, 179, 0, nb_pixels_diagonale, "resultats/route0_Hough.pgm");
    SavePPM_rgb8matrix(I_ligne, nrl, nrh, ncl, nch, "resultats/route0_ligne.ppm");


    // Liberation de la memoire
    printf("\nLiberation de la memoire...\n");
    free_bmatrix(I, nrl, nrh, ncl, nch);
    free_bmatrix(I_bin, nrl, nrh, ncl, nch);
    free_bmatrix(I_Hough, 0, 179, 0, nb_pixels_diagonale);
    free_rgb8matrix(I_ligne, nrl, nrh, ncl, nch);

    printf("Termine avec succes!\n");

    return 0;
}
