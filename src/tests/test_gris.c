/* ============================================================================
 *  test_gris.c  --  test de non-regression de rgb8_vers_gris
 *  ---------------------------------------------------------------------------
 *  C'est le seul test du projet qui s'appuie sur une VERITE TERRAIN externe :
 *  la base fournit, pour une partie des images, a la fois le .ppm couleur et
 *  le .pgm en niveaux de gris produit par l'encadrant. Notre conversion doit
 *  reproduire ce .pgm.
 *
 *  Les .pgm fournis ont ete generes avec les coefficients Rec. 709
 *  (COEF_LUMA_* dans traitement.h). Avec le Rec. 601, ce test tombe a ~24 %
 *  de pixels identiques sur une image couleur : il detecte donc immediatement
 *  un changement de formule, une erreur d'arrondi ou une inversion de canal.
 *
 *  Usage :  ./test_gris [numero ...]
 *  Sans argument, teste une selection d'images grises et couleur.
 *  Les fichiers doivent exister dans images/archive500ppm et archive500pgm.
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "nrio.h"
#include "nralloc.h"
#include "traitement.h"

/* Criteres d'acceptation : l'egalite parfaite n'est pas exigee, l'outil qui a
 * genere les .pgm n'arrondissait pas exactement comme nous.                 */
#define MIN_PCT_IDENTIQUE  99.5
#define ECART_TOLERE       1

static const char *NUMEROS_PAR_DEFAUT[] = {
    "1", "3", "17", "42", "57", "83", "99", "102", "120", "130"
};
#define NB_PAR_DEFAUT 10

/* Compare une image pour un numero donne. Renvoie 0 si conforme, 1 sinon.
 * Cumule les compteurs globaux passes par adresse.                         */
static int comparer(const char *numero, long *cumul_px, long *cumul_ok)
{
    char   chemin_ppm[256], chemin_pgm[256];
    long   r1, r2, c1, c2;       /* bornes du .ppm */
    long   r3, r4, c3, c4;       /* bornes du .pgm */
    long   i, j, n = 0, ok = 0, ecart_max = 0;
    rgb8 **couleur;
    byte **reference, **calcule;
    double pct;
    int    conforme;

    snprintf(chemin_ppm, sizeof(chemin_ppm), "images/archive500ppm/%s.ppm", numero);
    snprintf(chemin_pgm, sizeof(chemin_pgm), "images/archive500pgm/%s.pgm", numero);

    /* Rappel : NRC ne renvoie pas NULL si le fichier manque, il fait exit(1) */
    couleur   = LoadPPM_rgb8matrix(chemin_ppm, &r1, &r2, &c1, &c2);
    reference = LoadPGM_bmatrix(chemin_pgm,    &r3, &r4, &c3, &c4);
    calcule   = rgb8_vers_gris(couleur, r1, r2, c1, c2);

    if (r2 - r1 != r4 - r3 || c2 - c1 != c4 - c3) {
        printf("  %-8s  TAILLES DIFFERENTES entre le .ppm et le .pgm\n", numero);
        free_rgb8matrix(couleur, r1, r2, c1, c2);
        free_bmatrix(reference, r3, r4, c3, c4);
        free_bmatrix(calcule, r1, r2, c1, c2);
        return 1;
    }

    for (i = r1; i <= r2; i++) {
        for (j = c1; j <= c2; j++) {
            long ecart = (long)calcule[i][j] - (long)reference[i - r1 + r3][j - c1 + c3];
            if (ecart < 0) ecart = -ecart;
            if (ecart == 0) ok++;
            if (ecart > ecart_max) ecart_max = ecart;
            n++;
        }
    }

    pct      = 100.0 * (double)ok / (double)n;
    conforme = (pct >= MIN_PCT_IDENTIQUE && ecart_max <= ECART_TOLERE);

    printf("  %-8s %8ld px  %11.2f%% identiques   ecart max %ld   %s\n",
           numero, n, pct, ecart_max, conforme ? "ok" : "*** ECHEC ***");

    *cumul_px += n;
    *cumul_ok += ok;

    free_rgb8matrix(couleur, r1, r2, c1, c2);
    free_bmatrix(reference, r3, r4, c3, c4);
    free_bmatrix(calcule, r1, r2, c1, c2);

    return conforme ? 0 : 1;
}

int main(int argc, char *argv[])
{
    long cumul_px = 0, cumul_ok = 0;
    int  echecs = 0, nb = 0, i;

    printf("=== Test de rgb8_vers_gris contre les .pgm fournis ===\n");
    printf("seuils : >= %.1f%% de pixels identiques, ecart max %d niveau\n\n",
           MIN_PCT_IDENTIQUE, ECART_TOLERE);

    if (argc > 1) {
        for (i = 1; i < argc; i++, nb++)
            echecs += comparer(argv[i], &cumul_px, &cumul_ok);
    } else {
        for (i = 0; i < NB_PAR_DEFAUT; i++, nb++)
            echecs += comparer(NUMEROS_PAR_DEFAUT[i], &cumul_px, &cumul_ok);
    }

    printf("\n  %d image(s), %.4f%% des %ld pixels identiques\n",
           nb, 100.0 * (double)cumul_ok / (double)cumul_px, cumul_px);
    printf("  %s\n\n", echecs == 0 ? "TOUT EST CONFORME"
                                   : "*** DES IMAGES NE PASSENT PAS ***");

    return echecs == 0 ? 0 : 1;
}
