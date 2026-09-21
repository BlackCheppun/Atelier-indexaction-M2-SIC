/* ============================================================================
 *  tp1.c  --  TP : binarisation, etiquetage en composantes connexes et
 *             caracterisation d'une region (rectangle englobant, barycentre).
 *
 *  Les fonctions generiques (produit_convolution, binarisation) ont ete
 *  deplacees dans la bibliotheque src/lib/traitement.c : on les reutilise ici
 *  au lieu de les redefinir.
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"

#include "traitement.h"

byte **etiquettage(byte **image, long nrl, long nrh, long ncl, long nch){
    byte **newImage;
    newImage = bmatrix(nrl, nrh, ncl, nch);
    int etiquette=0;
    int att_A,att_B,att_C, ett_A, ett_B;
    for (int i = nrl; i <= nrh ; i++) {
        for (int j = ncl; j <= nch ; j++) {
            att_C=(int) image[i][j];
            if(att_C==255){
                if(i>nrl){
                    att_B=(int) image[i-1][j];
                    ett_B=(int) newImage[i-1][j];
                }else{
                    att_B= 0;
                    ett_B=etiquette;
                }
                if(j>ncl){
                    att_A= (int) image[i][j-1];
                    ett_A= (int) newImage[i][j-1];
                }else{
                    att_A=0;
                    ett_A=etiquette;
                }

                if (att_C == att_A && att_C != att_B) {
                    // 1. Si att(C)=att(A) et att(C)!=att(B)
                    newImage[i][j] = (byte) ett_A;
                }
                else if (att_C == att_B && att_C != att_A) {
                    // 2. Si att(C)=att(B) et att(C)!=att(A)
                    newImage[i][j] = (byte) ett_B;
                }
                else if (att_C != att_B && att_C != att_A) {
                    // 3. Si att(C)!=att(B) et att(C)!=att(A)
                    newImage[i][j] = (etiquette <= 255) ? (byte)etiquette : (byte)255;
                    etiquette++;
                }
                else if (att_C == att_B && att_C == att_A && ett_A == ett_B) {
                    // 4. Si att(C)=att(B)=att(A) et E(A)=E(B)
                    newImage[i][j] = (byte) ett_B;
                }
                else if (att_C == att_B && att_C == att_A && ett_A != ett_B) {
                    // 5. Si att(C)=att(B)=att(A) et E(A)!=E(B)
                    newImage[i][j] = (byte) ett_B;

                     // Remonter dans l'image et changer toutes les E(A) en E(B)
                    for (int k = nrl; k < i; k++) {
                        for (int l = ncl; l <=nch ; l++) {
                            if(newImage[k][l]==(byte) ett_A){
                               newImage[k][l]=(byte) ett_B;
                            }
                        }
                    }
                    for (int m =ncl; m<j ; m++){
                        if(newImage[i][m]==(byte) ett_A){
                            newImage[i][m]=(byte) ett_B;
                        }

                    }

                }

            }else{
                newImage[i][j]=(byte) 0;
            }

        }
    }
    return newImage;

}

byte **recherche_etiquette(byte **image,int etiquette, long nrl, long nrh, long ncl, long nch){
    byte **newImage;
    newImage = bmatrix(nrl, nrh, ncl, nch);

    for (int i = nrl; i <= nrh ; i++) {
        for (int j = ncl; j <= nch ; j++) {
            if((int) image[i][j] == etiquette){
                newImage[i][j]=(byte) 255;
            }else{
                newImage[i][j]=(byte) 0;
            }
        }
    }

    return newImage;


}

rgb8 **recherche_etiquette_et_caracterisation(
    byte **image_originale,      // Image en niveaux de gris
    byte **image_etiquettes,     // Image des etiquettes
    int etiquette,
    long nrl, long nrh,
    long ncl, long nch
)
{
    // Variables pour le calcul du centre de gravite
    long sum_x = 0, sum_y = 0;
    int count = 0;

    // Variables pour le rectangle englobant
    long min_x = nch, max_x = ncl;
    long min_y = nrh, max_y = nrl;

    // Parcours de l'image pour calculer les caracteristiques
    for (int i = nrl; i <= nrh; i++) {
        for (int j = ncl; j <= nch; j++) {
            if ((int)image_etiquettes[i][j] == etiquette) {
                // Accumulation pour le centre de gravite
                sum_x += j;
                sum_y += i;
                count++;

                // Mise a jour du rectangle englobant
                if (j < min_x) min_x = j;
                if (j > max_x) max_x = j;
                if (i < min_y) min_y = i;
                if (i > max_y) max_y = i;
            }
        }
    }

    // Calcul du centre de gravite
    int centre_x = 0, centre_y = 0;
    if (count > 0) {
        centre_x = sum_x / count;
        centre_y = sum_y / count;
    }

    // Affichage des resultats
    printf("\n=== Caracterisation de l'etiquette %d ===\n", etiquette);
    printf("Nombre de pixels : %d\n", count);
    printf("Centre de gravite : (%d, %d)\n", centre_x, centre_y);
    printf("Rectangle englobant : [%ld, %ld] x [%ld, %ld]\n", min_x, max_x, min_y, max_y);

    // Creation de l'image RGB avec rgb8matrix
    rgb8 **image_rgb = rgb8matrix(nrl, nrh, ncl, nch);

    // Remplissage de l'image RGB
    for (int i = nrl; i <= nrh; i++) {
        for (int j = ncl; j <= nch; j++) {
            byte pixel = image_originale[i][j];

            // Par defaut : niveau de gris sur les 3 canaux
            image_rgb[i][j].r = pixel;
            image_rgb[i][j].g = pixel;
            image_rgb[i][j].b = pixel;

            // Dessin du rectangle rouge
            if ((i == min_y || i == max_y) && (j >= min_x && j <= max_x)) {
                image_rgb[i][j].r = 255;
                image_rgb[i][j].g = 0;
                image_rgb[i][j].b = 0;
            }
            else if ((j == min_x || j == max_x) && (i >= min_y && i <= max_y)) {
                image_rgb[i][j].r = 255;
                image_rgb[i][j].g = 0;
                image_rgb[i][j].b = 0;
            }

            // Dessin du barycentre (petite croix rouge de 3x3 pixels)
            if (abs(i - centre_y) <= 1 && abs(j - centre_x) <= 1) {
                if (i == centre_y || j == centre_x) {
                    image_rgb[i][j].r = 255;
                    image_rgb[i][j].g = 0;
                    image_rgb[i][j].b = 0;
                }
            }
        }
    }

    return image_rgb;
}

int main(void) {
    long nrh, nrl, nch, ncl;
    byte **I;
    byte **I_bin;
    byte **I_etq;
    byte **I_region;
    rgb8 **I_rgb;

    printf("=== TP1 - Binarisation et Etiquetage ===\n\n");

    // Lecture de l'image avec LoadPGM_bmatrix
    printf("Lecture de l'image images/rice.pgm...\n");
    I = LoadPGM_bmatrix("images/rice.pgm", &nrl, &nrh, &ncl, &nch);

    if (I == NULL) {
        printf("Erreur: impossible de lire l'image images/rice.pgm\n");
        return 1;
    }

    printf("Image chargee: %ld x %ld pixels\n", nrh - nrl + 1, nch - ncl + 1);

    // Binarisation (fonction de la bibliotheque src/lib/traitement.c)
    printf("\nApplication de la binarisation...\n");
    I_bin = binarisation(I, 120, nrl, nrh, ncl, nch);

    // Etiquetage
    printf("Application de l'etiquetage...\n");
    I_etq = etiquettage(I_bin, nrl, nrh, ncl, nch);

    // Recherche d'une region specifique
    I_region = recherche_etiquette(I_etq, 100, nrl, nrh, ncl, nch);

    // Caracterisation de la region avec rectangle englobant
    printf("\nCaracterisation de la region...\n");
    I_rgb = recherche_etiquette_et_caracterisation(I, I_etq, 100, nrl, nrh, ncl, nch);

    // Sauvegarde des images
    printf("\nSauvegarde des images...\n");
    SavePGM_bmatrix(I_bin, nrl, nrh, ncl, nch, "resultats/rice_bin.pgm");
    SavePGM_bmatrix(I_etq, nrl, nrh, ncl, nch, "resultats/rice_etq.pgm");
    SavePGM_bmatrix(I_region, nrl, nrh, ncl, nch, "resultats/rice_region.pgm");
    SavePPM_rgb8matrix(I_rgb, nrl, nrh, ncl, nch, "resultats/rice_region_caracterisation.ppm");

    // Liberation de la memoire
    printf("\nLiberation de la memoire...\n");
    free_bmatrix(I, nrl, nrh, ncl, nch);
    free_bmatrix(I_bin, nrl, nrh, ncl, nch);
    free_bmatrix(I_etq, nrl, nrh, ncl, nch);
    free_bmatrix(I_region, nrl, nrh, ncl, nch);
    free_rgb8matrix(I_rgb, nrl, nrh, ncl, nch);

    printf("Termine avec succes!\n");

    return 0;
}
