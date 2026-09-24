/* caracteristiques.h -- extraction des caracteristiques d'une image */
#ifndef CARACTERISTIQUES_H
#define CARACTERISTIQUES_H

#include "def.h"

#define NB_NIVEAUX 256   /* nombre de classes des histogrammes */

/* Un pixel dont max(R,G,B) est sous ce seuil est trop sombre pour que sa
 * saturation ait un sens : elle y est indefinie ou dominee par le bruit. */
#define SEUIL_PIXEL_SOMBRE 15

/* Un pixel est dit "colore" s'il verifie les deux conditions :
 *   ecart (max - min) > SEUIL_ECART_COLORE
 *   saturation (max - min) / max > SEUIL_SATURATION_PIXEL */
#define SEUIL_ECART_COLORE      12
#define SEUIL_SATURATION_PIXEL  0.12

/* Proportion de pixels colores a partir de laquelle l'image est couleur. */
#define SEUIL_TAUX_COLORE       0.15

/* Bornes theoriques utilisees pour ramener dans [0,1]. */
#define LUMINANCE_MAX  255.0
#define CONTRASTE_MAX  127.5

/* Seuil sur la norme du gradient au-dela duquel un pixel est un contour. */
#define SEUIL_CONTOUR  100

/* --- histogrammes --- */

void histogramme_gris(byte **image, long nrl, long nrh, long ncl, long nch,
                      long hist[NB_NIVEAUX]);

void histogramme_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                     long hr[NB_NIVEAUX], long hg[NB_NIVEAUX], long hb[NB_NIVEAUX]);

/* Comptages -> frequences, pour comparer des images de tailles differentes. */
void normaliser_histogramme(const long hist[NB_NIVEAUX], long nb_pixels,
                            double hist_norm[NB_NIVEAUX]);

/* --- statistiques --- */

double moyenne_imatrix(int **m, long nrl, long nrh, long ncl, long nch);

double moyenne_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch);
double ecart_type_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne);

/* --- luminance et contraste, sur l'echelle [0,1] --- */

double normaliser_luminance(byte **m, long nrl, long nrh, long ncl, long nch);
double normaliser_contraste(byte **m, long nrl, long nrh, long ncl, long nch);

/* --- texture --- */

/* Taux de texturation dans [0,1] */
double taux_texture(byte **gris, long nrl, long nrh, long ncl, long nch,
                    int seuil_contour);

/* Nombre de pixels a 255 dans une image binaire de contours. */
long compter_pixels_contour(byte **contours, long nrl, long nrh, long ncl, long nch);

/* --- couleur --- */

/* Taux de rouge, vert et bleu  */
void taux_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
              double *taux_r, double *taux_g, double *taux_b);

double taux_rouge(rgb8 **image, long nrl, long nrh, long ncl, long nch);
double taux_vert (rgb8 **image, long nrl, long nrh, long ncl, long nch);
double taux_bleu (rgb8 **image, long nrl, long nrh, long ncl, long nch);

/* Moyenne de (max-min)/max sur les pixels assez lumineux. 0 = gris. */
double saturation_moyenne(rgb8 **image, long nrl, long nrh, long ncl, long nch);

/* 1 si couleur, 0 si noir et blanc */
int image_est_couleur(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                      double seuil_taux_colores);

#endif /* CARACTERISTIQUES_H */
