/* test_texture.c -- taux de texturation : verifie les valeurs et le classement.
 * Usage : ./test_texture [image.ppm ...] */
#include <stdio.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"
#include "caracteristiques.h"

/* Tolerance : les references sont donnees a 4 decimales.                   */
#define TOLERANCE 0.0005

typedef struct {
    const char *chemin;
    double      attendu;
} Reference;

static const Reference REFERENCES[] = {
    { "images/archive10ppm/arbre1.ppm", 0.6032 },
    { "images/archive10ppm/mer1.ppm",   0.4795 },
    { "images/archive10ppm/arbre3.ppm", 0.4298 },
    { "images/archive10ppm/arbre2.ppm", 0.3790 },
    { "images/archive10ppm/bus2.ppm",   0.3554 },
    { "images/archive10ppm/bus1.ppm",   0.2281 },
    { "images/archive10ppm/vache1.ppm", 0.2258 },
    { "images/archive10ppm/mer2.ppm",   0.1978 },
    { "images/archive10ppm/vache2.ppm", 0.1951 },
    { "images/archive10ppm/vache3.ppm", 0.1835 }
};
#define NB_REFERENCES 10

/* Calcule le taux de texture d'un fichier PPM. */
static double mesurer(const char *chemin)
{
    long   nrl, nrh, ncl, nch;
    rgb8 **couleur;
    byte  **gris;
    double taux;

    /* Rappel : NRC fait exit(1) si le fichier est introuvable.             */
    couleur = LoadPPM_rgb8matrix((char *)chemin, &nrl, &nrh, &ncl, &nch);
    gris    = rgb8_vers_gris(couleur, nrl, nrh, ncl, nch);

    taux = taux_texture(gris, nrl, nrh, ncl, nch, 0);

    free_rgb8matrix(couleur, nrl, nrh, ncl, nch);
    free_bmatrix(gris, nrl, nrh, ncl, nch);

    return taux;
}

int main(int argc, char *argv[])
{
    int    i, echecs = 0;
    double precedent = 2.0;   /* pour verifier que le classement tient      */

    printf("=== Test taux_texture (seuil par defaut = %d) ===\n\n", SEUIL_CONTOUR);

    if (argc > 1) {
        for (i = 1; i < argc; i++)
            printf("  %-34s %8.4f\n", argv[i], mesurer(argv[i]));
        printf("\n");
        return 0;
    }

    printf("  %-34s %8s %8s   %s\n", "image", "mesure", "attendu", "verdict");

    for (i = 0; i < NB_REFERENCES; i++) {
        double taux = mesurer(REFERENCES[i].chemin);
        double ecart = fabs(taux - REFERENCES[i].attendu);
        int    ok    = (ecart <= TOLERANCE);

        if (!ok) echecs++;

        printf("  %-34s %8.4f %8.4f   %s\n",
               REFERENCES[i].chemin, taux, REFERENCES[i].attendu,
               ok ? "ok" : "*** ECART ***");

        /* le tableau est trie par valeur decroissante : le classement doit
           se retrouver a l'identique */
        if (taux > precedent) {
            printf("      ^^^ CLASSEMENT ROMPU : cette image passe devant la precedente\n");
            echecs++;
        }
        precedent = taux;
    }

    printf("\n  %s\n\n", echecs == 0
           ? "TOUT EST CONFORME (valeurs et classement)"
           : "*** DES ECARTS ONT ETE DETECTES ***");

    return echecs == 0 ? 0 : 1;
}
