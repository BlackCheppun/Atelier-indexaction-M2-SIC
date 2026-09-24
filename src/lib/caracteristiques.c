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

/* Moyenne d'une imatrix. Utilisee sur la norme du gradient, dont elle donne
 * le niveau global de texture.
 *
 * ATTENTION AUX BORNES : la convolution ne peut pas calculer les bords, qui
 * valent donc 0 dans la norme du gradient. Les inclure tirerait la moyenne
 * vers le bas, et d'autant plus que l'image est petite -- donc inegalement
 * d'une image a l'autre. Appeler avec l'INTERIEUR :
 *     moyenne_imatrix(norme, nrl + 1, nrh - 1, ncl + 1, nch - 1)            */
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

/* Ecart-type d'une imatrix, en deux passes : la moyenne est recue en
 * parametre. Sur la norme du gradient, il mesure si la texture est repartie
 * uniformement (ecart-type proche de la moyenne) ou concentree sur quelques
 * contours francs (ecart-type nettement superieur a la moyenne).
 *
 * Doit etre appelee avec les MEMES bornes que la moyenne fournie.          */
double ecart_type_imatrix(int **m, long nrl, long nrh, long ncl, long nch,
                          double moyenne)
{
    double somme = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) return 0.0;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            double ecart = (double)m[i][j] - moyenne;
            somme += ecart * ecart;
        }
    }

    return sqrt(somme / (double)nb_pixels);
}

/* Luminance moyenne d'une image en niveaux de gris (matrice de byte, telle
 * que LoadPGM_bmatrix la rend, ou telle que rgb8_vers_gris la produit).
 * Resultat dans [0, 255] : 0 = image noire, 255 = image blanche.
 *
 * Contrairement aux fonctions couleur, AUCUN pixel n'est ecarte : un pixel
 * noir est une information de luminance parfaitement valide, c'est meme lui
 * qui fait qu'une image est sombre.                                        */
double moyenne_bmatrix(byte **m, long nrl, long nrh, long ncl, long nch)
{
    double somme = 0.0;
    long   nb_pixels;
    long   i, j;

    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);
    if (nb_pixels <= 0) return 0.0;

    for (i = nrl; i <= nrh; i++) {
        for (j = ncl; j <= nch; j++) {
            /* accumulation en double : exacte jusqu'a 2^53, alors qu'un long
               ne fait que 32 bits sous Windows */
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

/* Luminance de l'image ramenee de [0, 255] vers [0, 1].
 * On divise par une borne THEORIQUE et non par le maximum observe sur la
 * base : une image soumise ensuite par l'utilisateur est alors directement
 * comparable aux lignes deja stockees, sans avoir a recalculer la table.   */
double normaliser_luminance(byte **m, long nrl, long nrh, long ncl, long nch)
{
    return moyenne_bmatrix(m, nrl, nrh, ncl, nch) / LUMINANCE_MAX;
}

/* Contraste de l'image ramene de [0, 127.5] vers [0, 1].
 * 127.5 est le maximum mathematique de l'ecart-type pour des valeurs dans
 * [0,255] : il correspond a une image moitie noire moitie blanche.
 * Sur notre base, le maximum observe est 113.4, soit 0.89 une fois
 * normalise -- l'echelle est donc bien occupee.
 *
 * L'ecart-type se calculant par rapport a la moyenne, celle-ci est obtenue
 * ici meme : la fonction parcourt donc l'image deux fois.                  */
double normaliser_contraste(byte **m, long nrl, long nrh, long ncl, long nch)
{
    double moyenne = moyenne_bmatrix(m, nrl, nrh, ncl, nch);

    return ecart_type_bmatrix(m, nrl, nrh, ncl, nch, moyenne) / CONTRASTE_MAX;
}

/* Taux de texturation. Voir caracteristiques.h pour le detail du calcul et
 * la limite connue (seuil en niveaux de gris absolus).
 *
 * La chaine du gradient est faite ici : l'appelant fournit l'image grise et
 * n'a ni matrice a allouer ni bornes a resserrer.                          */
double taux_texture(byte **gris, long nrl, long nrh, long ncl, long nch,
                    int seuil_contour)
{
    long   i, j, nb_interieur, nb_contour = 0;
    double somme_sature = 0.0, resultat;
    int  **Ix, **Iy, **norme;

    if (nrh - nrl < 2 || nch - ncl < 2) return 0.0;   /* pas d'interieur */
    if (seuil_contour <= 0) seuil_contour = SEUIL_CONTOUR;

    Ix    = gradient_x(gris, nrl, nrh, ncl, nch);
    Iy    = gradient_y(gris, nrl, nrh, ncl, nch);
    norme = norme_gradient(Ix, Iy, nrl, nrh, ncl, nch);

    /* les bords valent 0 (convolution non calculable) : on les exclut */
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

    /* moyenne de l'amplitude plafonnee et de la densite de contours */
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
    /* TODO : cumuler R, G, B puis diviser chacun par (R+G+B)                */
    (void)image; (void)nrl; (void)nrh; (void)ncl; (void)nch;
    if (taux_r) *taux_r = 0.0;
    if (taux_g) *taux_g = 0.0;
    if (taux_b) *taux_b = 0.0;
}

/* Saturation moyenne au sens HSV : moyenne de (max-min)/max sur les pixels
 * suffisamment lumineux.
 *   0.0 -> gris parfait (R=G=B partout)
 *   1.0 -> couleurs pleinement saturees
 * Les pixels sombres (max < SEUIL_PIXEL_SOMBRE) sont ecartes : la saturation
 * y est indefinie (max == 0) ou numeriquement instable, un ecart d'un seul
 * niveau y produisant deja un rapport eleve.                                */
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

            /* division flottante : en entiers elle vaudrait 0 partout */
            somme += (double)(mx - mn) / (double)mx;
            nb_pixels_retenus++;
        }
    }

    if (nb_pixels_retenus == 0) return 0.0;   /* image entierement sombre */

    return somme / (double)nb_pixels_retenus;
}

/* Decide si l'image est en couleur a partir de la PROPORTION de pixels
 * colores (et non de la saturation moyenne, qui est aveugle aux petites
 * zones colorees sur fond gris).
 * Le taux est calcule ici meme, en une seule passe sur l'image.            */
int image_est_couleur(rgb8 **image, long nrl, long nrh, long ncl, long nch,
                      double seuil_taux_colores)
{
    long nb_colores  = 0;   /* pixels franchement colores                   */
    long nb_pixels_retenus  = 0;   /* pixels assez lumineux pour etre juges        */
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

            /* les deux conditions sont necessaires : l'ecart absolu ecarte
               le bruit de chrominance, le rapport ecarte les teintes pales */
            if ((mx - mn) > SEUIL_ECART_COLORE &&
                (double)(mx - mn) / (double)mx > SEUIL_SATURATION_PIXEL) {
                nb_colores++;
            }
        }
    }

    if (nb_pixels_retenus == 0) return 0;   /* image entierement sombre -> N&B     */

    return ((double)nb_colores / (double)nb_pixels_retenus > seuil_taux_colores)
           ? 1 : 0;
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
