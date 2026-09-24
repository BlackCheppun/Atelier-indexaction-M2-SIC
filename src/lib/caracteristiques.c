/* caracteristiques.c -- calcul des caracteristiques d'une image */
#include <stdio.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "caracteristiques.h"

void histogramme_gris(byte **image, long nrl, long nrh, long ncl, long nch,
                      long hist[NB_NIVEAUX])
{
    long i, j;

    for (i = 0; i < NB_NIVEAUX; i++) {
        hist[i] = 0;
    }

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

    for (i = 0; i < NB_NIVEAUX; i++) {
        hr[i] = 0;
        hg[i] = 0;
        hb[i] = 0;
    }

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

    for (i = 0; i < NB_NIVEAUX; i++) {
        hist_norm[i] = (double)hist[i] / (double)nb_pixels;
    }
}


double moyenne_imatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
    double somme = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) return 0.0;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            somme += (double)m[i][j];
        }
    }

    return somme / (double)nb_pixels;
}


double moyenne_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch)
{
    double somme = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) return 0.0;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            somme += (double)m[i][j];
        }
    }

    return somme / (double)nb_pixels;
}


double ecart_type_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne)
{
    double somme = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) return 0.0;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            somme += ((double)m[i][j] - moyenne) * ((double)m[i][j] - moyenne);
        }
    }
    return sqrt(somme / (double)nb_pixels);
}

double normaliser_luminance(byte **m, long nrl, long nrh, long ncl, long nch)
{
    return moyenne_bmatrix(m, nrl, nrh, ncl, nch) / LUMINANCE_MAX;
}

double normaliser_contraste(byte **m, long nrl, long nrh, long ncl, long nch)
{
    double moyenne = moyenne_bmatrix(m, nrl, nrh, ncl, nch);

    return ecart_type_bmatrix(m, nrl, nrh, ncl, nch, moyenne) / CONTRASTE_MAX;
}

double taux_texture(byte **gris, long nrl, long nrh, long ncl, long nch,
                    int seuil_contour)
{
    long   i, j, nb_interieur, nb_contour = 0;
    double somme_sature = 0.0, resultat;
    int  **Ix, **Iy, **norme;

    if (nrh - nrl < 2 || nch - ncl < 2) return 0.0;
    if (seuil_contour <= 0) seuil_contour = SEUIL_CONTOUR;

    Ix    = gradient_x(gris, nrl, nrh, ncl, nch);
    Iy    = gradient_y(gris, nrl, nrh, ncl, nch);
    norme = norme_gradient(Ix, Iy, nrl, nrh, ncl, nch);

    /* bords exclus : la convolution les laisse a 0 */
    for (i = nrl + 1; i <= nrh - 1; i++) {
        for (j = ncl + 1; j <= nch - 1; j++) {
            int n = norme[i][j];

            if (n > seuil_contour) {
                nb_contour++;
                somme_sature += 1.0;              /* amplitude plafonnee */
            } else {
                somme_sature += (double)n / (double)seuil_contour;
            }
        }
    }
    nb_interieur = (nrh - nrl - 1) * (nch - ncl - 1);

    resultat = 0.5 * (somme_sature / (double)nb_interieur
                      + (double)nb_contour / (double)nb_interieur);

    free_imatrix(Ix, nrl, nrh, ncl, nch);
    free_imatrix(Iy, nrl, nrh, ncl, nch);
    free_imatrix(norme, nrl, nrh, ncl, nch);

    return resultat;
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
    double somme_r = 0.0, somme_g = 0.0, somme_b = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) {
        if (taux_r) *taux_r = 1.0 / 3.0;
        if (taux_g) *taux_g = 1.0 / 3.0;
        if (taux_b) *taux_b = 1.0 / 3.0;
        return;
    }

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            double r = (double)image[i][j].r;
            double g = (double)image[i][j].g;
            double b = (double)image[i][j].b;
            double total = r + g + b;

            if (total > 0.0) {
                somme_r += r / total;
                somme_g += g / total;
                somme_b += b / total;
            } else {
                /* pixel noir : 0/0 donnerait un NaN, aucune dominante */
                somme_r += 1.0 / 3.0;
                somme_g += 1.0 / 3.0;
                somme_b += 1.0 / 3.0;
            }
        }
    }

    if (taux_r) *taux_r = somme_r / (double)nb_pixels;
    if (taux_g) *taux_g = somme_g / (double)nb_pixels;
    if (taux_b) *taux_b = somme_b / (double)nb_pixels;
}

double taux_rouge(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    double r, g, b;
    taux_rgb(image, nrl, nrh, ncl, nch, &r, &g, &b);
    return r;
}

double taux_vert(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    double r, g, b;
    taux_rgb(image, nrl, nrh, ncl, nch, &r, &g, &b);
    return g;
}

double taux_bleu(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    double r, g, b;
    taux_rgb(image, nrl, nrh, ncl, nch, &r, &g, &b);
    return b;
}


double saturation_moyenne(rgb8 **image, long nrl, long nrh, long ncl, long nch)
{
    double somme = 0.0;
    long   nb_pixels_retenus = 0;
    long   i, j;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            /* en int : .r .g .b sont des byte non signes, et on soustrait */
            int r = (int)image[i][j].r;
            int g = (int)image[i][j].g;
            int b = (int)image[i][j].b;

            int mx = (r > g) ? r : g;
            if (b > mx) mx = b;

            if (mx < SEUIL_PIXEL_SOMBRE) continue;

            int mn = (r < g) ? r : g;
            if (b < mn) mn = b;

            somme += (double)(mx - mn) / (double)mx;
            nb_pixels_retenus++;
        }
    }

    if (nb_pixels_retenus == 0) return 0.0;

    return somme / (double)nb_pixels_retenus;
}

/* Decision prise sur la proportion de pixels colores, calculee ici meme. */
int image_est_couleur(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                      double seuil_taux_colores)
{
    long nb_colores = 0;
    long nb_pixels_retenus = 0;
    long i, j;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            int r = (int)image[i][j].r;
            int g = (int)image[i][j].g;
            int b = (int)image[i][j].b;

            int mx = (r > g) ? r : g;
            if (b > mx) mx = b;

            if (mx < SEUIL_PIXEL_SOMBRE) continue;

            int mn = (r < g) ? r : g;
            if (b < mn) mn = b;

            nb_pixels_retenus++;

            if ((mx - mn) > SEUIL_ECART_COLORE &&
                (double)(mx - mn) / (double)mx > SEUIL_SATURATION_PIXEL) {
                nb_colores++;
            }
        }
    }

    if (nb_pixels_retenus == 0) return 0;

    return ((double)nb_colores / (double)nb_pixels_retenus > seuil_taux_colores)
           ? 1 : 0;
}
