/* test_taux_rgb.c -- taux de rouge, vert et bleu : verifie les valeurs, la
 * somme a 1 et la coherence des accesseurs.
 * Usage : ./test_taux_rgb [image.ppm ...] */
#include <stdio.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "caracteristiques.h"

#define TOLERANCE      0.0005
#define TOLERANCE_SOMME 1e-9

typedef struct {
    const char *chemin;
    double      r, g, b;
} Reference;

static const Reference REFERENCES[] = {
    { "images/archive10ppm/arbre1.ppm", 0.2751, 0.3270, 0.3980 },
    { "images/archive10ppm/vache1.ppm", 0.3881, 0.3895, 0.2223 },
    { "images/archive10ppm/bus1.ppm",   0.4127, 0.2569, 0.3304 },
    { "images/archive10ppm/mer2.ppm",   0.3539, 0.3000, 0.3461 },
    { "images/archive500ppm/231.ppm",   0.5759, 0.2239, 0.2002 },
    { "images/archive500ppm/447.ppm",   0.1029, 0.4418, 0.4554 },
    { "images/archive500ppm/83.ppm",    0.3389, 0.3291, 0.3320 },
    { "images/archive500ppm/140.ppm",   0.3333, 0.3333, 0.3333 }
};
#define NB_REFERENCES 8

/* Renvoie 0 si tout est conforme, 1 sinon. */
static int verifier(const Reference *ref)
{
    long   nrl, nrh, ncl, nch;
    rgb8 **image;
    double r, g, b, somme;
    int    valeurs_ok, somme_ok, accesseurs_ok;

    image = LoadPPM_rgb8matrix((char *)ref->chemin, &nrl, &nrh, &ncl, &nch);

    taux_rgb(image, nrl, nrh, ncl, nch, &r, &g, &b);
    somme = r + g + b;

    valeurs_ok = (fabs(r - ref->r) <= TOLERANCE)
              && (fabs(g - ref->g) <= TOLERANCE)
              && (fabs(b - ref->b) <= TOLERANCE);

    somme_ok = (fabs(somme - 1.0) <= TOLERANCE_SOMME);

    accesseurs_ok =
        (fabs(taux_rouge(image, nrl, nrh, ncl, nch) - r) < 1e-12) &&
        (fabs(taux_vert (image, nrl, nrh, ncl, nch) - g) < 1e-12) &&
        (fabs(taux_bleu (image, nrl, nrh, ncl, nch) - b) < 1e-12);

    printf("  %-30s %7.4f %7.4f %7.4f   %s%s%s\n",
           ref->chemin, r, g, b,
           valeurs_ok    ? "ok " : "*** VALEUR *** ",
           somme_ok      ? ""    : "*** SOMME != 1 *** ",
           accesseurs_ok ? ""    : "*** ACCESSEURS *** ");

    free_rgb8matrix(image, nrl, nrh, ncl, nch);

    return (valeurs_ok && somme_ok && accesseurs_ok) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    int i, echecs = 0;

    printf("=== Test taux_rgb / taux_rouge / taux_vert / taux_bleu ===\n\n");

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            long nrl, nrh, ncl, nch;
            double r, g, b;
            rgb8 **image = LoadPPM_rgb8matrix(argv[i], &nrl, &nrh, &ncl, &nch);
            taux_rgb(image, nrl, nrh, ncl, nch, &r, &g, &b);
            printf("  %-30s %7.4f %7.4f %7.4f\n", argv[i], r, g, b);
            free_rgb8matrix(image, nrl, nrh, ncl, nch);
        }
        printf("\n");
        return 0;
    }

    printf("  %-30s %7s %7s %7s   %s\n",
           "image", "rouge", "vert", "bleu", "verdict");

    for (i = 0; i < NB_REFERENCES; i++)
        echecs += verifier(&REFERENCES[i]);

    printf("\n  %s\n\n", echecs == 0
           ? "TOUT EST CONFORME (valeurs, somme = 1, accesseurs)"
           : "*** DES ECARTS ONT ETE DETECTES ***");

    return echecs == 0 ? 0 : 1;
}
