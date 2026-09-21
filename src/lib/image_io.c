/* ============================================================================
 *  image_io.c  --  implementation du module d'entree/sortie
 *  Etat : [FAIT] (sauf la conversion couleur->gris qui vit dans traitement.c)
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "image_io.h"

const char *nom_de_base(const char *chemin)
{
    const char *s1 = strrchr(chemin, '/');
    const char *s2 = strrchr(chemin, '\\');
    const char *s  = (s1 > s2) ? s1 : s2;
    return s ? s + 1 : chemin;
}

int est_ppm(const char *chemin)
{
    size_t n = strlen(chemin);
    if (n < 4) return 0;
    return (tolower((unsigned char)chemin[n-3]) == 'p' &&
            tolower((unsigned char)chemin[n-2]) == 'p' &&
            tolower((unsigned char)chemin[n-1]) == 'm' &&
            chemin[n-4] == '.');
}

int charger_image(const char *chemin, Image *img)
{
    if (img == NULL) return -1;

    img->gris    = NULL;
    img->couleur = NULL;

    if (est_ppm(chemin)) {
        img->couleur = LoadPPM_rgb8matrix((char *)chemin,
                                          &img->nrl, &img->nrh,
                                          &img->ncl, &img->nch);
        if (img->couleur == NULL) return -1;

        img->gris = rgb8_vers_gris(img->couleur,
                                   img->nrl, img->nrh, img->ncl, img->nch);
        if (img->gris == NULL) {           /* rgb8_vers_gris pas encore ecrite */
            free_rgb8matrix(img->couleur, img->nrl, img->nrh, img->ncl, img->nch);
            img->couleur = NULL;
            return -1;
        }
    } else {
        img->gris = LoadPGM_bmatrix((char *)chemin,
                                    &img->nrl, &img->nrh,
                                    &img->ncl, &img->nch);
        if (img->gris == NULL) return -1;
    }

    img->largeur   = img->nch - img->ncl + 1;
    img->hauteur   = img->nrh - img->nrl + 1;
    img->nb_pixels = img->largeur * img->hauteur;
    return 0;
}

void liberer_image(Image *img)
{
    if (img == NULL) return;
    if (img->gris) {
        free_bmatrix(img->gris, img->nrl, img->nrh, img->ncl, img->nch);
        img->gris = NULL;
    }
    if (img->couleur) {
        free_rgb8matrix(img->couleur, img->nrl, img->nrh, img->ncl, img->nch);
        img->couleur = NULL;
    }
}

int lister_images(const char *dossier, char noms[][MAX_CHEMIN], int max)
{
    DIR *d;
    struct dirent *e;
    int n = 0;

    d = opendir(dossier);
    if (d == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le dossier %s\n", dossier);
        return 0;
    }

    while ((e = readdir(d)) != NULL && n < max) {
        size_t len = strlen(e->d_name);
        if (len < 5) continue;
        if (strcmp(e->d_name + len - 4, ".pgm") != 0 &&
            strcmp(e->d_name + len - 4, ".ppm") != 0) continue;

        snprintf(noms[n], MAX_CHEMIN, "%s/%s", dossier, e->d_name);
        n++;
    }

    closedir(d);
    return n;
}
