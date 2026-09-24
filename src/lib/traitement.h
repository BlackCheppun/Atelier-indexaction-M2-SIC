#ifndef TRAITEMENT_H
#define TRAITEMENT_H

#include "def.h"

extern int MASQUE_MOYENNEUR[3][3];
extern int MASQUE_SOBEL_X[3][3];
extern int MASQUE_SOBEL_Y[3][3];

byte **produit_convolution(byte **image, long nrl, long nrh, long ncl, long nch,
                           int masque[3][3], int diviseur);

int **convolution_signee(byte **image, long nrl, long nrh, long ncl, long nch,
                         int masque[3][3], int diviseur);

int **norme_gradient(int **Ix, int **Iy, long nrl, long nrh, long ncl, long nch);

byte **imatrix_vers_bmatrix(int **m, long nrl, long nrh, long ncl, long nch);

byte **seuillage_imatrix(int **m, int seuil,
                         long nrl, long nrh, long ncl, long nch);

#define COEF_LUMA_R  0.2126
#define COEF_LUMA_G  0.7152
#define COEF_LUMA_B  0.0722

byte **rgb8_vers_gris(rgb8 **image, long nrl, long nrh, long ncl, long nch);

int **gradient_x(byte **image, long nrl, long nrh, long ncl, long nch);
int **gradient_y(byte **image, long nrl, long nrh, long ncl, long nch);

#endif
