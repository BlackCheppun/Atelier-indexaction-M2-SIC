/* ============================================================================
 *  test_couleur.c  --  test des fonctions couleur / noir et blanc
 *  ---------------------------------------------------------------------------
 *  Charge des images PPM avec NRC et affiche, pour chacune :
 *      - la saturation moyenne
 *      - la decision est_couleur
 *
 *  Usage :  ./test_couleur [image1.ppm image2.ppm ...]
 *  Sans argument, teste les 3 images de images/test/.
 * ==========================================================================*/
#include <stdio.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "caracteristiques.h"

static const char *IMAGES_PAR_DEFAUT[] = {
    "images/test/Aerial512.ppm",
    "images/test/Embryos512.ppm",
    "images/test/Sailboat512.ppm"
};
#define NB_PAR_DEFAUT 3

static void tester(const char *chemin)
{
    long   nrl, nrh, ncl, nch;
    rgb8 **image;
    double saturation;
    int    couleur;

    /* Attention : si le fichier n'existe pas, NRC n'echoue pas proprement,
     * il appelle nrerror() qui fait exit(1). Le test ci-dessous ne sert donc
     * a rien en pratique -- le processus est deja mort. Les chemins doivent
     * utiliser des '/' (le conteneur est sous Linux).                       */
    image = LoadPPM_rgb8matrix((char *)chemin, &nrl, &nrh, &ncl, &nch);
    if (image == NULL) {
        printf("  %-28s  LECTURE IMPOSSIBLE\n", chemin);
        return;
    }

    saturation = saturation_moyenne(image, nrl, nrh, ncl, nch);
    couleur    = image_est_couleur(image, nrl, nrh, ncl, nch, SEUIL_TAUX_COLORE);

    printf("  %-28s  %3ldx%-3ld   saturation = %.4f   est_couleur = %d   -> %s\n",
           chemin,
           nch - ncl + 1, nrh - nrl + 1,
           saturation, couleur,
           couleur ? "COULEUR" : "noir et blanc");

    free_rgb8matrix(image, nrl, nrh, ncl, nch);
}

int main(int argc, char *argv[])
{
    int i;

    printf("=== Test saturation_moyenne / image_est_couleur ===\n");
    printf("seuils : pixel sombre < %d | ecart > %d | saturation pixel > %.2f"
           " | taux colore > %.2f\n\n",
           SEUIL_PIXEL_SOMBRE, SEUIL_ECART_COLORE,
           SEUIL_SATURATION_PIXEL, SEUIL_TAUX_COLORE);

    if (argc > 1) {
        for (i = 1; i < argc; i++) tester(argv[i]);
    } else {
        for (i = 0; i < NB_PAR_DEFAUT; i++) tester(IMAGES_PAR_DEFAUT[i]);
    }

    printf("\n");
    return 0;
}
