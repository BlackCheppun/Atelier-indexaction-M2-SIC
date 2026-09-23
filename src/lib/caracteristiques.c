/* ============================================================================
 *  caracteristiques.c  --  implementation du module de caracterisation
 *  ---------------------------------------------------------------------------
 *  Etat : la plomberie (export CSV, affichage) est ecrite ; les fonctions de
 *  calcul sont a implementer (marquees TODO).
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "image_io.h"
#include "caracteristiques.h"

/* ==========================================================================
 *  Histogrammes
 * ========================================================================*/

void histogramme_gris(byte **image, long nrl, long nrh, long ncl, long nch,
                      long hist[NB_NIVEAUX])
{
    long i, j;

    /* 1. Initialisation : mettre tout l'histogramme à 0 */
    for (i = 0; i < NB_NIVEAUX; i++) {
        hist[i] = 0;
    }

    /* 2. Comptage : on incrémente la case correspondant à la valeur du pixel */
    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            hist[image[i][j]]++;
        }
    }
}

void histogramme_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                     long hr[NB_NIVEAUX], long hg[NB_NIVEAUX], long hb[NB_NIVEAUX])
{
    long i, j;

    /* 1. Initialisation */
    for (i = 0; i < NB_NIVEAUX; i++) {
        hr[i] = 0;
        hg[i] = 0;
        hb[i] = 0;
    }

    /* 2. Comptage pour chaque canal */
    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            hr[image[i][j].r]++;
            hg[image[i][j].g]++;
            hb[image[i][j].b]++;
        }
    }
}

void normaliser_histogramme(const long hist[NB_NIVEAUX], long nb_pixels,
                            double hist_norm[NB_NIVEAUX])
{
    int i;
    /* Division par le nombre total de pixels pour avoir des fréquences */
    for (i = 0; i < NB_NIVEAUX; i++) {
        hist_norm[i] = (double)hist[i] / (double)nb_pixels;
    }
}

void reduire_histogramme(const double hist_norm[NB_NIVEAUX],
                         double hist_reduit[NB_BINS])
{
    /* TODO : regrouper NB_NIVEAUX/NB_BINS classes consecutives (somme)      */
    (void)hist_norm; (void)hist_reduit;
}

int sauver_histogramme_txt(const long hist[NB_NIVEAUX], const char *fichier)
{
    /* TODO : ecrire NB_NIVEAUX lignes "niveau <tab> effectif"               */
    (void)hist; (void)fichier;
        return -1;
    }

/* ==========================================================================
 *  Caracteristiques scalaires
 * ========================================================================*/

double moyenne_imatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
    /* TODO */
    (void)m; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return 0.0;
}

double ecart_type_imatrix(int **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne)
{
    /* TODO : sqrt( somme((m[i][j]-moyenne)^2) / N )                         */
    (void)m; (void)nrl; (void)nrh; (void)ncl; (void)nch; (void)moyenne;
    return 0.0;
}

double moyenne_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch)
{
    /* TODO */
    (void)m; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return 0.0;
}

double ecart_type_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne)
{
    /* TODO */
    (void)m; (void)nrl; (void)nrh; (void)ncl; (void)nch; (void)moyenne;
    return 0.0;
}

long compter_pixels_contour(byte **contours, long nrl, long nrh, long ncl, long nch)
{
    long count = 0;
    long i, j;
    
    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            if (contours[i][j] == 255) {
                count++;
            }
        }
    }
    return count;
}

void taux_rgb(rgb8 **image, long nrl, long nrh, long ncl, long nch,
              double *taux_r, double *taux_g, double *taux_b)
{
    /* TODO : cumuler R, G, B puis diviser chacun par (R+G+B)                */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    if (taux_r) *taux_r = 0.0;
    if (taux_g) *taux_g = 0.0;
    if (taux_b) *taux_b = 0.0;
}

double saturation_moyenne(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    /* TODO : par pixel  s = (max(r,g,b) - min(r,g,b)) / max(r,g,b) si max>0 */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    return 0.0;
}

int image_est_couleur(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                      double seuil_saturation)
{
    /* TODO : return saturation_moyenne(...) > seuil_saturation              */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    (void)seuil_saturation;
    return 0;
}

double entropie_histogramme(const double hist_norm[NB_NIVEAUX])
{
    /* TODO : -somme( p * log2(p) ) en ignorant les p nuls                   */
    (void)hist_norm;
    return 0.0;
}

/* ==========================================================================
 *  Distances
 * ========================================================================*/

double distance_L1(const double h1[], const double h2[], int taille)
{
    /* TODO : somme des |h1[k] - h2[k]|                                      */
    (void)h1; (void)h2; (void)taille;
    return 0.0;
}

double distance_L2(const double h1[], const double h2[], int taille)
{
    /* TODO : sqrt( somme (h1[k]-h2[k])^2 )                                  */
    (void)h1; (void)h2; (void)taille;
    return 0.0;
}

double distance_intersection(const double h1[], const double h2[], int taille)
{
    /* TODO : 1 - somme( min(h1[k], h2[k]) )                                 */
    (void)h1; (void)h2; (void)taille;
    return 0.0;
}

double distance_bhattacharyya(const double h1[], const double h2[], int taille)
{
    /* TODO : BC = somme sqrt(h1[k]*h2[k])   puis   d = sqrt(1 - BC)         */
    (void)h1; (void)h2; (void)taille;
    return 0.0;
}

double distance_chi2(const double h1[], const double h2[], int taille)
{
    /* TODO : somme (h1-h2)^2 / (h1+h2), en ignorant les denominateurs nuls  */
    (void)h1; (void)h2; (void)taille;
    return 0.0;
}

double distance_descripteurs(const Descripteur *d1, const Descripteur *d2,
                             double poids_couleur, double poids_texture)
{
    /* TODO : combiner par exemple
     *   couleur = bhattacharyya(hist_r) + bhattacharyya(hist_g) + (hist_b)
     *   texture = |gradient_moyen1 - gradient_moyen2| normalise
     *   retour  = poids_couleur*couleur + poids_texture*texture
     */
    (void)d1; (void)d2; (void)poids_couleur; (void)poids_texture;
    return 0.0;
}

/* ==========================================================================
 *  Fonction chapeau
 * ========================================================================*/

int calculer_descripteur(const char *chemin, int seuil_contour, Descripteur *d)
{
    /* TODO : enchainement complet
     *   1. charger_image(chemin, ...)  -> version gris + version couleur
     *   2. histogrammes (gris, et R/G/B si couleur) + normalisation
     *   3. luminance, contraste, entropie
     *   4. Ix = gradient_x(gris), Iy = gradient_y(gris)
     *      N  = norme_gradient(Ix, Iy)
     *      gradient_moyen / ecart-type = moyenne/ecart_type_imatrix(N)
     *      contours = seuillage_imatrix(N, seuil_contour)
     *      nb_pixels_contour, taux_contour
     *   5. couleur : moyennes R/G/B, taux_rgb, saturation, est_couleur
     *   6. TOUT liberer (free_bmatrix / free_imatrix / free_rgb8matrix)
     */
    (void)chemin; (void)seuil_contour; (void)d;
    return -1;
}

/* ==========================================================================
 *  Export CSV (interface avec la Partie II / ORACLE)  --  [FAIT]
 * ========================================================================*/

void csv_ecrire_entete(FILE *f)
{
    int k;
    fprintf(f, "nom;largeur;hauteur;nb_pixels;est_couleur;"
               "moyenne_r;moyenne_g;moyenne_b;taux_r;taux_g;taux_b;"
               "saturation;luminance;contraste;entropie;"
               "gradient_moyen;gradient_ecart_type;nb_pixels_contour;taux_contour");
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";hg%d", k);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";hr%d", k);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";hv%d", k);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";hb%d", k);
    fprintf(f, "\n");
}

void csv_ecrire_descripteur(FILE *f, const Descripteur *d)
{
    double hg[NB_BINS], hr[NB_BINS], hv[NB_BINS], hb[NB_BINS];
    int k;

    reduire_histogramme(d->hist_gris, hg);
    reduire_histogramme(d->hist_r,    hr);
    reduire_histogramme(d->hist_g,    hv);
    reduire_histogramme(d->hist_b,    hb);

    fprintf(f, "%s;%ld;%ld;%ld;%d;"
               "%.4f;%.4f;%.4f;%.6f;%.6f;%.6f;"
               "%.6f;%.4f;%.4f;%.4f;"
               "%.4f;%.4f;%ld;%.6f",
            d->nom, d->largeur, d->hauteur, d->nb_pixels, d->est_couleur,
            d->moyenne_r, d->moyenne_g, d->moyenne_b,
            d->taux_r, d->taux_g, d->taux_b,
            d->saturation_moyenne, d->luminance_moyenne, d->contraste, d->entropie,
            d->gradient_moyen, d->gradient_ecart_type,
            d->nb_pixels_contour, d->taux_contour);

    for (k = 0; k < NB_BINS; k++) fprintf(f, ";%.6f", hg[k]);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";%.6f", hr[k]);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";%.6f", hv[k]);
    for (k = 0; k < NB_BINS; k++) fprintf(f, ";%.6f", hb[k]);
    fprintf(f, "\n");
}

void afficher_descripteur(const Descripteur *d)
{
    printf("--- %s (%ldx%ld, %s) ---\n", d->nom, d->largeur, d->hauteur,
           d->est_couleur ? "couleur" : "noir et blanc");
    printf("  luminance=%.2f  contraste=%.2f  entropie=%.2f\n",
           d->luminance_moyenne, d->contraste, d->entropie);
    printf("  gradient moyen=%.2f (+/-%.2f)  contours=%ld (%.2f%%)\n",
           d->gradient_moyen, d->gradient_ecart_type,
           d->nb_pixels_contour, 100.0 * d->taux_contour);
    printf("  taux R/V/B = %.3f / %.3f / %.3f   saturation=%.3f\n",
           d->taux_r, d->taux_g, d->taux_b, d->saturation_moyenne);
}
