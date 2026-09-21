/* ============================================================================
 *  extraction.c  --  Programme principal de la PARTIE I
 *  ---------------------------------------------------------------------------
 *  Parcourt un dossier d'images (.pgm / .ppm), calcule le descripteur de
 *  chaque image et produit un fichier CSV charge ensuite dans ORACLE
 *  (Partie II).
 *
 *  Usage :  ./extraction [dossier_images] [fichier_csv] [seuil_contour]
 *  Defaut :  images/  resultats/caracteristiques.csv  100
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "image_io.h"
#include "caracteristiques.h"

int main(int argc, char *argv[])
{
    const char *dossier = (argc > 1) ? argv[1] : "images";
    const char *sortie  = (argc > 2) ? argv[2] : "resultats/caracteristiques.csv";
    int seuil_contour   = (argc > 3) ? atoi(argv[3]) : 100;

    char noms[MAX_IMAGES][MAX_CHEMIN];
    Descripteur d;
    FILE *f;
    int nb, i, ok = 0;

    printf("=== Partie I : extraction des caracteristiques ===\n");
    printf("Dossier : %s | seuil contours : %d\n\n", dossier, seuil_contour);

    nb = lister_images(dossier, noms, MAX_IMAGES);
    if (nb == 0) {
        printf("Aucune image .pgm/.ppm trouvee dans %s\n", dossier);
        return 1;
    }
    printf("%d image(s) trouvee(s).\n\n", nb);

    f = fopen(sortie, "w");
    if (f == NULL) {
        printf("Impossible d'ecrire %s\n", sortie);
        return 1;
    }
    csv_ecrire_entete(f);

    for (i = 0; i < nb; i++) {
        memset(&d, 0, sizeof(d));

        if (calculer_descripteur(noms[i], seuil_contour, &d) != 0) {
            printf("[ECHEC] %s\n", noms[i]);
            continue;
        }

        afficher_descripteur(&d);
        csv_ecrire_descripteur(f, &d);
        ok++;
    }

    fclose(f);
    printf("\n%d/%d image(s) traitee(s) -> %s\n", ok, nb, sortie);
    return 0;
}
