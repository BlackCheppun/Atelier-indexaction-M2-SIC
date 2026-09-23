/* ============================================================================
 *  caracteristiques.h  --  Bibliotheque "Atelier Indexation d'images"
 *  ---------------------------------------------------------------------------
 *  Module 2/3 : extraction des CARACTERISTIQUES (descripteurs) d'une image,
 *  et distances entre descripteurs. C'est le coeur de la Partie I : ce sont
 *  ces valeurs qui seront stockees dans la base ORACLE (Partie II).
 * ==========================================================================*/
#ifndef CARACTERISTIQUES_H
#define CARACTERISTIQUES_H

#include <stdio.h>
#include "def.h"

#define NB_NIVEAUX 256   /* nombre de classes des histogrammes             */
#define NB_BINS    16    /* histogramme reduit (16 classes) pour la base    */

/* Un pixel dont max(R,G,B) est sous ce seuil est trop sombre pour que sa
 * saturation ait un sens : on l'ecarte des calculs couleur.                */
#define SEUIL_PIXEL_SOMBRE 20

/* Un pixel est dit "colore" s'il satisfait LES DEUX conditions :
 *   - ecart absolu  (max - min) > SEUIL_ECART_COLORE
 *   - saturation    (max - min) / max > SEUIL_SATURATION_PIXEL
 * La condition absolue filtre le bruit de chrominance des scans et des JPEG
 * (amplitude de quelques niveaux), qui produit un rapport eleve dans les
 * zones sombres alors que le pixel est gris a l'oeil.                      */
#define SEUIL_ECART_COLORE      12
#define SEUIL_SATURATION_PIXEL  0.12

/* Proportion de pixels colores a partir de laquelle l'image est declaree
 * en couleur (valeur par defaut a passer a image_est_couleur).             */
#define SEUIL_TAUX_COLORE       0.02

/* --------------------------------------------------------------------------
 *  Le descripteur d'une image : une ligne de la future table ORACLE.
 * ------------------------------------------------------------------------*/
typedef struct {
    char   nom[256];                  /* nom du fichier image               */
    long   largeur, hauteur;
    long   nb_pixels;

    int    est_couleur;               /* 1 = couleur, 0 = niveaux de gris   */

    /* --- couleur --- */
    double moyenne_r, moyenne_g, moyenne_b;   /* 0..255                     */
    double taux_r, taux_g, taux_b;            /* R/(R+G+B), somme = 1       */
    double saturation_moyenne;                /* 0..1, sert au N&B          */

    /* --- luminance / contraste --- */
    double luminance_moyenne;                 /* moyenne des niveaux de gris*/
    double contraste;                         /* ecart-type des niv. de gris*/
    double entropie;                          /* desordre de l'histogramme  */

    /* --- texture / contours --- */
    double gradient_moyen;                    /* moyenne de la norme        */
    double gradient_ecart_type;
    long   nb_pixels_contour;                 /* apres seuillage            */
    double taux_contour;                      /* nb_contour / nb_pixels     */

    /* --- histogrammes normalises (somme = 1) --- */
    double hist_gris[NB_NIVEAUX];
    double hist_r[NB_NIVEAUX];
    double hist_g[NB_NIVEAUX];
    double hist_b[NB_NIVEAUX];
} Descripteur;

/* ==========================================================================
 *  [A FAIRE] Histogrammes                                              (2.6)
 * ========================================================================*/

/* Histogramme brut (comptage) d'une image en niveaux de gris.              */
void histogramme_gris(byte **image, long nrl, long nrh, long ncl, long nch,
                      long hist[NB_NIVEAUX]);

/* Histogrammes bruts des 3 canaux d'une image couleur.                     */
void histogramme_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                     long hr[NB_NIVEAUX], long hg[NB_NIVEAUX], long hb[NB_NIVEAUX]);

/* Passage comptage -> frequences (somme = 1). Indispensable pour comparer
 * des images de tailles differentes.                                       */
void normaliser_histogramme(const long hist[NB_NIVEAUX], long nb_pixels,
                            double hist_norm[NB_NIVEAUX]);

/* Reduction a NB_BINS classes (utile pour un VARRAY ORACLE compact).       */
void reduire_histogramme(const double hist_norm[NB_NIVEAUX],
                         double hist_reduit[NB_BINS]);

/* Sauvegarde de l'histogramme dans un fichier texte "niveau valeur".  (2.6)*/
int sauver_histogramme_txt(const long hist[NB_NIVEAUX], const char *fichier);

/* ==========================================================================
 *  [A FAIRE] Caracteristiques scalaires                                (2.8)
 * ========================================================================*/

/* Moyenne / ecart-type d'une imatrix (utilises sur la norme du gradient).  */
double moyenne_imatrix(int **m, long nrl, long nrh, long ncl, long nch);
double ecart_type_imatrix(int **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne);

/* Moyenne / ecart-type d'une image en niveaux de gris.                     */
double moyenne_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch);
double ecart_type_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne);

/* Nombre de pixels a 255 dans une image binaire (pixels de contour).       */
long compter_pixels_contour(byte **contours, long nrl, long nrh, long ncl, long nch);

/* Taux de rouge / vert / bleu NORMALISES : taux_r = R/(R+G+B).
 * Attention : c'est ce qui permet de ne pas considerer une image blanche
 * comme "tres rouge" (piege signale dans le sujet, 2.8).                   */
void taux_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
              double *taux_r, double *taux_g, double *taux_b);

/* Saturation moyenne (modele HSV simplifie) : (max-min)/max par pixel.
 * Sert a decider si une image est reellement en couleur ou en N&B.         */
double saturation_moyenne(rgb8 **image, long nrl, long nrh, long ncl, long nch);

/* 1 si l'image est consideree couleur, 0 si noir et blanc.
 * La decision repose sur la PROPORTION de pixels colores, pas sur la
 * saturation moyenne : une image massivement grise comportant une petite
 * zone franchement coloree (capture d'ecran, objet colore sur fond gris)
 * garde une moyenne faible et serait classee N&B a tort.
 * Le taux est calcule directement ici, en une seule passe.
 * seuil_taux_colores : proportion declenchante, ex. SEUIL_TAUX_COLORE.     */
int image_est_couleur(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                      double seuil_taux_colores);

/* Entropie de Shannon de l'histogramme normalise : -somme(p*log2(p)).      */
double entropie_histogramme(const double hist_norm[NB_NIVEAUX]);

/* ==========================================================================
 *  [A FAIRE] Distances entre histogrammes                              (2.8)
 * ========================================================================*/
double distance_L1(const double h1[], const double h2[], int taille);
double distance_L2(const double h1[], const double h2[], int taille);
double distance_intersection(const double h1[], const double h2[], int taille);
double distance_bhattacharyya(const double h1[], const double h2[], int taille);
double distance_chi2(const double h1[], const double h2[], int taille);

/* Distance globale entre deux images (combinaison ponderee couleur/texture).*/
double distance_descripteurs(const Descripteur *d1, const Descripteur *d2,
                             double poids_couleur, double poids_texture);

/* ==========================================================================
 *  [A FAIRE] Fonction "chapeau" et export
 * ========================================================================*/

/* Calcule TOUTES les caracteristiques d'un fichier image (.pgm ou .ppm).
 * Retourne 0 si OK, -1 en cas d'erreur.                                    */
int calculer_descripteur(const char *chemin, int seuil_contour, Descripteur *d);

/* Export CSV : c'est le fichier qui sera charge dans ORACLE (Partie II).   */
void csv_ecrire_entete(FILE *f);
void csv_ecrire_descripteur(FILE *f, const Descripteur *d);

/* Affichage lisible dans le terminal (debug / demo).                       */
void afficher_descripteur(const Descripteur *d);

#endif /* CARACTERISTIQUES_H */
