/* traitement.h -- filtrage, gradient et conversions
 * Convention NRC : matrices indexees de [nrl..nrh][ncl..nch]. */
#ifndef TRAITEMENT_H
#define TRAITEMENT_H

#include "def.h"

/* Masques de convolution 3x3 */
extern int MASQUE_MOYENNEUR[3][3];   /* passe-bas, diviseur = 9 */
extern int MASQUE_SOBEL_X[3][3];     /* gradient horizontal -> Ix */
extern int MASQUE_SOBEL_Y[3][3];     /* gradient vertical   -> Iy */

/* Convolution 3x3 affichable : valeur absolue, saturee a 255, bords recopies. */
byte **produit_convolution(byte **image, long nrl, long nrh, long ncl, long nch,
                           int masque[3][3], int diviseur);

/* Convolution conservant le signe et sans saturation, necessaire au gradient.
 * Retourne une imatrix, a liberer avec free_imatrix. Bords mis a 0. */
int **convolution_signee(byte **image, long nrl, long nrh, long ncl, long nch,
                         int masque[3][3], int diviseur);

/* Norme du gradient : sqrt(Ix^2 + Iy^2). */
int **norme_gradient(int **Ix, int **Iy, long nrl, long nrh, long ncl, long nch);

/* Mise a l'echelle [min..max] -> [0..255], pour sauvegarder en PGM. */
byte **imatrix_vers_bmatrix(int **m, long nrl, long nrh, long ncl, long nch);

/* Seuillage d'une imatrix -> image binaire 0 / 255 (detection de contours). */
byte **seuillage_imatrix(int **m, int seuil,
                         long nrl, long nrh, long ncl, long nch);

/* Conversion couleur -> niveaux de gris.
 * Coefficients Rec. 709 (primaires sRGB) : ce sont ceux qui ont servi a
 * produire les .pgm fournis avec la base, que la conversion reproduit. */
#define COEF_LUMA_R  0.2126
#define COEF_LUMA_G  0.7152
#define COEF_LUMA_B  0.0722

byte **rgb8_vers_gris(rgb8 **image, long nrl, long nrh, long ncl, long nch);

int **gradient_x(byte **image, long nrl, long nrh, long ncl, long nch);
int **gradient_y(byte **image, long nrl, long nrh, long ncl, long nch);

#endif /* TRAITEMENT_H */
