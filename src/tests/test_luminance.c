/* test_luminance.c -- luminance et contraste, valeur brute puis normalisee.
 * Usage : ./test_luminance [image.pgm ...] */
#include <stdio.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "caracteristiques.h"

static const char *IMAGES_PAR_DEFAUT[] = {
    "images/archive500pgm/1.pgm",
    "images/archive500pgm/42.pgm",
    "images/archive500pgm/83.pgm",
    "images/archive500pgm/99.pgm",
    "images/archive500pgm/120.pgm"
};
#define NB_PAR_DEFAUT 5

static void tester(const char *chemin)
{
    long   nrl, nrh, ncl, nch;
    byte **gris;
    double luminance, contraste;

    /* Rappel : si le fichier est introuvable, NRC appelle nrerror() qui fait
     * exit(1). Les chemins doivent utiliser des '/'.                        */
    gris = LoadPGM_bmatrix((char *)chemin, &nrl, &nrh, &ncl, &nch);
    if (gris == NULL) {
        printf("  %-32s  LECTURE IMPOSSIBLE\n", chemin);
        return;
    }

    luminance = moyenne_bmatrix(gris, nrl, nrh, ncl, nch);
    contraste = ecart_type_bmatrix(gris, nrl, nrh, ncl, nch, luminance);

    printf("  %-30s %3ldx%-3ld   luminance %8.4f (%.4f)   contraste %8.4f (%.4f)\n",
           chemin,
           nch - ncl + 1, nrh - nrl + 1,
           luminance, normaliser_luminance(gris, nrl, nrh, ncl, nch),
           contraste, normaliser_contraste(gris, nrl, nrh, ncl, nch));

    free_bmatrix(gris, nrl, nrh, ncl, nch);
}

int main(int argc, char *argv[])
{
    int i;

    printf("=== Test luminance et contraste  (valeur brute, puis normalisee) ===\n\n");

    if (argc > 1) {
        for (i = 1; i < argc; i++) tester(argv[i]);
    } else {
        for (i = 0; i < NB_PAR_DEFAUT; i++) tester(IMAGES_PAR_DEFAUT[i]);
        printf("\n  luminance attendue : 175.0098 | 169.9377 |  24.6539 | 152.6667 | 181.7703\n");
        printf("  contraste attendu  :  65.6610 |  55.0395 |  45.8834 |  53.1423 |  88.0019\n");
    }

    printf("\n");
    return 0;
}
