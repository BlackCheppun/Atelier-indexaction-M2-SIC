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
    /* TODO :
     *  - allouer   int **res = imatrix(nrl, nrh, ncl, nch);
     *  - bords (i == nrl/nrh, j == ncl/nch) : mettre 0 (pas de voisinage)
     *  - interieur : somme des produits, division par diviseur si != 0,
     *    PAS de abs(), PAS de saturation -> on garde le signe.
     */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    (void)masque; (void)diviseur;
    return NULL;
}

int **norme_gradient(int **Ix, int **Iy, long nrl, long nrh, long ncl, long nch)
{
    /* TODO : res[i][j] = (int)round(sqrt(Ix^2 + Iy^2))                      */
    (void)Ix; (void)Iy; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return NULL;
}

byte **imatrix_vers_bmatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
    /* TODO : chercher min et max, puis
     *        res[i][j] = 255 * (m[i][j] - min) / (max - min)  (si max != min)
     */
    (void)m; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return NULL;
}

byte **seuillage_imatrix(int **m, int seuil,
                         long nrl, long nrh, long ncl, long nch)
{
    /* TODO : m[i][j] >= seuil -> 255 (contour), sinon 0                     */
    (void)m; (void)seuil; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return NULL;
}

byte **rgb8_vers_gris(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    /* TODO : gris = 0.299*r + 0.587*g + 0.114*b  (arrondi, borne 0..255)    */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return NULL;
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
