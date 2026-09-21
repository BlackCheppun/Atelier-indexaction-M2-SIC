/* ============================================================================
 *  image_io.h  --  Bibliotheque "Atelier Indexation d'images"
 *  ---------------------------------------------------------------------------
 *  Module 3/3 : chargement generique d'une image (.pgm ou .ppm) et utilitaires
 *  de fichiers. Encapsule NRC pour que le reste du code n'ait pas a savoir si
 *  l'image d'origine etait en couleur ou en niveaux de gris.
 * ==========================================================================*/
#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "def.h"

#define MAX_IMAGES  512
#define MAX_CHEMIN  512

/* Une image chargee : toujours disponible en niveaux de gris, et en couleur
 * uniquement si le fichier d'origine etait un .ppm.                        */
typedef struct {
    byte **gris;        /* jamais NULL apres un chargement reussi           */
    rgb8 **couleur;     /* NULL si le fichier etait un .pgm                 */
    long   nrl, nrh, ncl, nch;
    long   largeur, hauteur, nb_pixels;
} Image;

/* Charge un .pgm ou un .ppm (detection par l'extension).
 * Pour un .ppm, remplit aussi le champ gris via rgb8_vers_gris().
 * Retourne 0 si OK, -1 sinon.                                              */
int  charger_image(const char *chemin, Image *img);

/* Libere toutes les matrices de la structure.                              */
void liberer_image(Image *img);

/* "images/chat.ppm" -> "chat.ppm"                                          */
const char *nom_de_base(const char *chemin);

/* Retourne 1 si le chemin se termine par .ppm (insensible a la casse).     */
int  est_ppm(const char *chemin);

/* Liste les fichiers .pgm/.ppm d'un dossier (tri non garanti).
 * Remplit noms[i] avec le chemin complet. Retourne le nombre trouve.       */
int  lister_images(const char *dossier, char noms[][MAX_CHEMIN], int max);

#endif /* IMAGE_IO_H */
