/* ============================================================================
 *  mass_conversion.c
 *  ---------------------------------------------------------------------------
 *  Calcule en masse les histogrammes et la densite des contours pour un 
 *  dossier d'images (.pgm et .ppm), puis genere des requetes SQL INSERT.
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "caracteristiques.h"

#define SEUIL_CONTOUR 100

/* Helper pour formater un tableau de double en chaine SQL (normalise) */
void formater_histogramme(const double hist[NB_NIVEAUX], char *buffer, size_t max_len) {
    char temp[32];
    buffer[0] = '\0';
    for (int i = 0; i < NB_NIVEAUX; i++) {
        snprintf(temp, sizeof(temp), "%.4f", hist[i]);
        strncat(buffer, temp, max_len - strlen(buffer) - 1);
        if (i < NB_NIVEAUX - 1) {
            strncat(buffer, ",", max_len - strlen(buffer) - 1);
        }
    }
}

void traiter_image(FILE *f_sql, const char *chemin, const char *nom_fichier) {
    int est_ppm = (strstr(nom_fichier, ".ppm") != NULL);

    if (!est_ppm) return;

    long nrh, nrl, nch, ncl;
    long nb_pixels;
    
    long hist_brut_gris[NB_NIVEAUX] = {0};
    long hr[NB_NIVEAUX] = {0};
    long hg[NB_NIVEAUX] = {0};
    long hb[NB_NIVEAUX] = {0};

    double hist_norm_gris[NB_NIVEAUX] = {0};
    double hist_norm_r[NB_NIVEAUX] = {0};
    double hist_norm_g[NB_NIVEAUX] = {0};
    double hist_norm_b[NB_NIVEAUX] = {0};

    byte **I_gris = NULL;
    rgb8 **I_couleur = NULL;
    int **Ix = NULL, **Iy = NULL, **norme = NULL;
    byte **contours = NULL;

    /* Traitement exclusif des images PPM */
    I_couleur = LoadPPM_rgb8matrix((char *)chemin, &nrl, &nrh, &ncl, &nch);
    if (!I_couleur) return;
    nb_pixels = (nrh - nrl + 1) * (nch - ncl + 1);

    /* Histogrammes RGB bruts et normalises */
    histogramme_rgb(I_couleur, nrl, nrh, ncl, nch, hr, hg, hb);
    normaliser_histogramme(hr, nb_pixels, hist_norm_r);
    normaliser_histogramme(hg, nb_pixels, hist_norm_g);
    normaliser_histogramme(hb, nb_pixels, hist_norm_b);
    
    /* Conversion en gris pour les contours et histo gris */
    I_gris = rgb8_vers_gris(I_couleur, nrl, nrh, ncl, nch);
    
    /* Histogramme gris brut et normalise */
    histogramme_gris(I_gris, nrl, nrh, ncl, nch, hist_brut_gris);
    normaliser_histogramme(hist_brut_gris, nb_pixels, hist_norm_gris);

    /* Calcul des contours */
    Ix = gradient_x(I_gris, nrl, nrh, ncl, nch);
    Iy = gradient_y(I_gris, nrl, nrh, ncl, nch);
    norme = norme_gradient(Ix, Iy, nrl, nrh, ncl, nch);
    contours = seuillage_imatrix(norme, SEUIL_CONTOUR, nrl, nrh, ncl, nch);

    long nb_pixels_contour = compter_pixels_contour(contours, nrl, nrh, ncl, nch);
    /* On laisse la densite de contour en double (taux), ou bien on met le nombre de pixels.
     * Si l'utilisateur veut aussi le vrai nombre de pixels pour la densite, on modifie.
     * L'utilisateur a precise : "pour les valeurs de l'histogramme, je veux avoir le vrai nombre de pixel" 
     * Donc la densite reste un taux ou le count absolu. Je vais formater le taux. */
    double densite = (double)nb_pixels_contour / (double)nb_pixels;

    double luminosite = normaliser_luminance(I_gris, nrl, nrh, ncl, nch);
    double contraste = normaliser_contraste(I_gris, nrl, nrh, ncl, nch);
    double saturation = saturation_moyenne(I_couleur, nrl, nrh, ncl, nch);
    int is_color = image_est_couleur(I_couleur, nrl, nrh, ncl, nch, SEUIL_TAUX_COLORE);

    /* Taux de rouge / vert / bleu : parts normalisees, de somme 1 */
    double taux_r, taux_g, taux_b;
    taux_rgb(I_couleur, nrl, nrh, ncl, nch, &taux_r, &taux_g, &taux_b);

    /* Generation de la requete SQL */
    char str_hg[2500], str_hr[2500], str_hg_color[2500], str_hb[2500];
    
    formater_histogramme(hist_norm_gris, str_hg, sizeof(str_hg));
    formater_histogramme(hist_norm_r, str_hr, sizeof(str_hr));
    formater_histogramme(hist_norm_g, str_hg_color, sizeof(str_hg_color));
    formater_histogramme(hist_norm_b, str_hb, sizeof(str_hb));

    /* Remplacement de .ppm par .jpg pour la base de données */
    char nom_db[256];
    strncpy(nom_db, nom_fichier, sizeof(nom_db));
    nom_db[sizeof(nom_db) - 1] = '\0';
    char *ext = strstr(nom_db, ".ppm");
    if (ext != NULL) {
        strcpy(ext, ".jpg");
    }

    fprintf(f_sql, "UPDATE TP_INDEXATION.IMAGES SET \n");
    fprintf(f_sql, "  HISTO_GRIS = HISTO_VARRAY(%s),\n", str_hg);
    fprintf(f_sql, "  HISTO_R = HISTO_VARRAY(%s),\n", str_hr);
    fprintf(f_sql, "  HISTO_G = HISTO_VARRAY(%s),\n", str_hg_color);
    fprintf(f_sql, "  HISTO_B = HISTO_VARRAY(%s),\n", str_hb);
    fprintf(f_sql, "  DENSITE_CONTOURS = %.6f,\n", densite);
    fprintf(f_sql, "  TEXTURE = %.6f,\n", contraste);
    fprintf(f_sql, "  LUMINOSITE_MOYENNE = %.6f,\n", luminosite);
    fprintf(f_sql, "  SATURATION_MOYENNE = %.6f,\n", saturation);
    fprintf(f_sql, "  TAUX_R = %.6f,\n", taux_r);
    fprintf(f_sql, "  TAUX_G = %.6f,\n", taux_g);
    fprintf(f_sql, "  TAUX_B = %.6f,\n", taux_b);
    fprintf(f_sql, "  IS_COLOR = %d\n", is_color);
    fprintf(f_sql, "WHERE NOM = '%s';\n\n", nom_db);

    /* Liberation memoire */
    if (I_gris) free_bmatrix(I_gris, nrl, nrh, ncl, nch);
    if (I_couleur) free_rgb8matrix(I_couleur, nrl, nrh, ncl, nch);
    if (Ix) free_imatrix(Ix, nrl, nrh, ncl, nch);
    if (Iy) free_imatrix(Iy, nrl, nrh, ncl, nch);
    if (norme) free_imatrix(norme, nrl, nrh, ncl, nch);
    if (contours) free_bmatrix(contours, nrl, nrh, ncl, nch);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <dossier_images>\n", argv[0]);
        return 1;
    }

    DIR *dir;
    struct dirent *ent;
    char chemin_complet[1024];

    FILE *f_sql = fopen("requetes_oracle.sql", "w");
    if (!f_sql) {
        perror("Impossible de creer le fichier requetes_oracle.sql");
        return EXIT_FAILURE;
    }

    if ((dir = opendir(argv[1])) != NULL) {
        /* Parcourir tous les fichiers du repertoire */
        while ((ent = readdir(dir)) != NULL) {
            /* Ignorer . et .. */
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }

            snprintf(chemin_complet, sizeof(chemin_complet), "%s/%s", argv[1], ent->d_name);
            traiter_image(f_sql, chemin_complet, ent->d_name);
        }
        closedir(dir);
    } else {
        perror("Impossible d'ouvrir le dossier");
        fclose(f_sql);
        return EXIT_FAILURE;
    }

    fprintf(f_sql, "COMMIT;\nEXIT;\n");
    fclose(f_sql);

    printf("Fichier requetes_oracle.sql genere avec succes.\n");
    printf("Lancement de l'insertion dans Oracle via sqlplus...\n");
    
    int ret = system("sqlplus tp_indexation/123456@localhost:1522/orcl @requetes_oracle.sql");
    if (ret == 0) {
        printf("Insertion terminee.\n");
    } else {
        printf("Erreur lors de l'insertion via sqlplus.\n");
    }

    return 0;
}
