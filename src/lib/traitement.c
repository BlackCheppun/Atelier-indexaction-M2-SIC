/* ============================================================================
 *  traitement.c  --  implementation du module de pre-traitement
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"

/* --------------------------------------------------------------------------
 *  Masques
 * ------------------------------------------------------------------------*/
int MASQUE_MOYENNEUR[3][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};

/* Gradient horizontal (derivee selon x) */
int MASQUE_SOBEL_X[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

/* Gradient vertical (derivee selon y) */
int MASQUE_SOBEL_Y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

/* ==========================================================================
 *  [FAIT] Repris de tp1.c / tp3.c
 * ========================================================================*/
byte **produit_convolution(byte **image, long nrl, long nrh, long ncl, long nch,
                           int masque[3][3], int diviseur)
{
    byte **newImage;
    newImage = bmatrix(nrl, nrh, ncl, nch);

    /* Traitement des pixels interieurs (en evitant les bords) */
    for (int i = nrl + 1; i <= nrh - 1; i++) {
        for (int j = ncl + 1; j <= nch - 1; j++) {
            int somme = 0;

            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    somme += (int)image[i - 1 + k][j - 1 + l] * masque[k][l];
                }
            }

            if (diviseur != 0) {
                somme = somme / diviseur;
            }
            somme = abs(somme);

            /* Gestion des debordements */
            if (somme > 255) somme = 255;
            if (somme < 0)   somme = 0;

            newImage[i][j] = (byte)somme;
        }
    }

    /* Copie des bords (premiere et derniere ligne) */
    for (int j = ncl; j <= nch; j++) {
        newImage[nrl][j] = image[nrl][j];
        newImage[nrh][j] = image[nrh][j];
    }

    /* Copie des bords (premiere et derniere colonne) */
    for (int i = nrl; i <= nrh; i++) {
        newImage[i][ncl] = image[i][ncl];
        newImage[i][nch] = image[i][nch];
    }

    return newImage;
}

byte **binarisation(byte **image, int seuil,
                    long nrl, long nrh, long ncl, long nch)
{
    byte **newImage;
    newImage = bmatrix(nrl, nrh, ncl, nch);

    for (int i = nrl; i <= nrh; i++) {
        for (int j = ncl; j <= nch; j++) {
            int current_pixel = (int)image[i][j];
            if (current_pixel < seuil) {
                newImage[i][j] = (byte)0;
            } else {
                newImage[i][j] = (byte)255;
            }
        }
    }
    return newImage;
}

/* ==========================================================================
 *  [A FAIRE] squelettes a completer
 * ========================================================================*/

int **convolution_signee(byte **image, long nrl, long nrh, long ncl, long nch,
                         int masque[3][3], int diviseur)
{
    int **res = imatrix(nrl, nrh, ncl, nch);
    int i, j, k, l;

    /* Interieur */
    for (i = nrl + 1; i <= nrh - 1; i++) {
        for (j = ncl + 1; j <= nch - 1; j++) {
            int somme = 0;
            for (k = 0; k < 3; k++) {
                for (l = 0; l < 3; l++) {
                    somme += (int)image[i - 1 + k][j - 1 + l] * masque[k][l];
                }
            }
            if (diviseur != 0) {
                somme = somme / diviseur;
            }
            res[i][j] = somme; /* Pas de valeur absolue, pas de saturation */
        }
    }

    /* Bords : mettre 0 (pas de calcul possible) */
    for (j = ncl; j <= nch; j++) {
        res[nrl][j] = 0;
        res[nrh][j] = 0;
    }
    for (i = nrl; i <= nrh; i++) {
        res[i][ncl] = 0;
        res[i][nch] = 0;
    }

    return res;
}

int **norme_gradient(int **Ix, int **Iy, long nrl, long nrh, long ncl, long nch)
{
    int **res = imatrix(nrl, nrh, ncl, nch);
    int i, j;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            res[i][j] = (int)round(sqrt((double)(Ix[i][j] * Ix[i][j] + Iy[i][j] * Iy[i][j])));
        }
    }
    return res;
}

byte **imatrix_vers_bmatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
    byte **res = bmatrix(nrl, nrh, ncl, nch);
    int i, j;
    int min_val = m[nrl][ncl], max_val = m[nrl][ncl];

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            if (m[i][j] < min_val) min_val = m[i][j];
            if (m[i][j] > max_val) max_val = m[i][j];
        }
    }

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            if (max_val != min_val) {
                res[i][j] = (byte)(255.0 * (m[i][j] - min_val) / (max_val - min_val));
            } else {
                res[i][j] = 0;
            }
        }
    }
    return res;
}

byte **seuillage_imatrix(int **m, int seuil,
                         long nrl, long nrh, long ncl, long nch)
{
    byte **res = bmatrix(nrl, nrh, ncl, nch);
    int i, j;
    
    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            if (m[i][j] >= seuil) {
                res[i][j] = (byte)255;
            } else {
                res[i][j] = (byte)0;
            }
        }
    }
    return res;
}

byte **rgb8_vers_gris(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    byte **res = bmatrix(nrl, nrh, ncl, nch);
    int i, j;
    
    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            double gris = 0.299 * image[i][j].r + 0.587 * image[i][j].g + 0.114 * image[i][j].b;
            int gris_int = (int)round(gris);
            if (gris_int > 255) gris_int = 255;
            if (gris_int < 0) gris_int = 0;
            res[i][j] = (byte)gris_int;
        }
    }
    return res;
}

byte **filtre_moyenneur(byte **image, long nrl, long nrh, long ncl, long nch)
{
    return produit_convolution(image, nrl, nrh, ncl, nch, MASQUE_MOYENNEUR, 9);
}

int **gradient_x(byte **image, long nrl, long nrh, long ncl, long nch)
{
    return convolution_signee(image, nrl, nrh, ncl, nch, MASQUE_SOBEL_X, 1);
}

int **gradient_y(byte **image, long nrl, long nrh, long ncl, long nch)
{
    return convolution_signee(image, nrl, nrh, ncl, nch, MASQUE_SOBEL_Y, 1);
}
