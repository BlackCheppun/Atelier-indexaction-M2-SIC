/* ============================================================================
 *  traitement.h  --  Bibliotheque "Atelier Indexation d'images" (Partie I)
 *  ---------------------------------------------------------------------------
 *  Module 1/3 : pre-traitements et operateurs de bas niveau.
 *
 *  Cette bibliotheque est INDEPENDANTE de NRC : elle s'appuie sur NRC
 *  (allocation / IO) mais aucun fichier de NRC/ ne doit etre modifie.
 *
 *  Convention NRC : toutes les matrices sont indexees de [nrl..nrh][ncl..nch]
 *  (nrl = num row low, nrh = num row high, ncl/nch = idem colonnes).
 * ==========================================================================*/
#ifndef TRAITEMENT_H
#define TRAITEMENT_H

#include "def.h"        /* byte, rgb8  (NRC) */

/* --------------------------------------------------------------------------
 *  Masques de convolution 3x3
 * ------------------------------------------------------------------------*/
extern int MASQUE_MOYENNEUR[3][3];   /* passe-bas, diviseur = 9            */
extern int MASQUE_SOBEL_X[3][3];     /* gradient horizontal -> Ix          */
extern int MASQUE_SOBEL_Y[3][3];     /* gradient vertical   -> Iy          */

/* ==========================================================================
 *  [FAIT] Fonctions deja ecrites en TP (tp1.c / tp3.c), simplement migrees
 * ========================================================================*/

/* Convolution 3x3 "affichable" : resultat en valeur absolue, sature a 255.
 * Les bords sont recopies depuis l'image source.
 * A utiliser pour VISUALISER (moyenneur, Sobel en pgm).           [tp1/tp3] */
byte **produit_convolution(byte **image, long nrl, long nrh, long ncl, long nch,
                           int masque[3][3], int diviseur);

/* Seuillage binaire : pixel < seuil -> 0, sinon 255.              [tp1/tp3] */
byte **binarisation(byte **image, int seuil,
                    long nrl, long nrh, long ncl, long nch);

/* ==========================================================================
 *  [A FAIRE] Fonctions a implementer pour l'atelier
 * ========================================================================*/

/* 1) Convolution SIGNEE : indispensable pour le gradient.
 *    produit_convolution() perd le signe (abs) et sature a 255 : on ne peut
 *    donc pas calculer sqrt(Ix^2+Iy^2) correctement a partir d'elle.
 *    Retourne une imatrix NRC (a liberer avec free_imatrix).           (2.4) */
int **convolution_signee(byte **image, long nrl, long nrh, long ncl, long nch,
                         int masque[3][3], int diviseur);

/* 2) Norme du gradient : N(x,y) = sqrt(Ix^2 + Iy^2), non saturee.     (2.4.3)*/
int **norme_gradient(int **Ix, int **Iy, long nrl, long nrh, long ncl, long nch);

/* 3) Conversion d'une imatrix quelconque en byte matrix affichable :
 *    mise a l'echelle [min..max] -> [0..255] pour sauvegarde PGM.           */
byte **imatrix_vers_bmatrix(int **m, long nrl, long nrh, long ncl, long nch);

/* 4) Seuillage d'une imatrix (norme du gradient) -> image binaire de contours
 *    0 / 255. C'est la detection de contours du sujet.                (2.5) */
byte **seuillage_imatrix(int **m, int seuil,
                         long nrl, long nrh, long ncl, long nch);

/* 5) Conversion couleur -> niveaux de gris : 0.299R + 0.587G + 0.114B (2.7) */
byte **rgb8_vers_gris(rgb8 **image, long nrl, long nrh, long ncl, long nch);

/* 6) Raccourcis de confort (wrappers sur les fonctions ci-dessus)          */
byte **filtre_moyenneur(byte **image, long nrl, long nrh, long ncl, long nch);
int  **gradient_x(byte **image, long nrl, long nrh, long ncl, long nch);
int  **gradient_y(byte **image, long nrl, long nrh, long ncl, long nch);

#endif /* TRAITEMENT_H */
