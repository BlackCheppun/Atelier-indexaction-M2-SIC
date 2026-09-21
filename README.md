# Atelier Indexation d'images — M2 IISC Pro

Recherche d'images par le contenu : extraction de caractéristiques en C (Partie I),
stockage/interrogation dans Oracle (Partie II), interface minimaliste de requête par
image exemple.

## Organisation du dépôt

```
NRC/            bibliothèque NRC fournie (def.h, nrio, nralloc, nrarith) — NE PAS MODIFIER
src/lib/        NOTRE bibliothèque, réutilisable par tous les programmes
                  traitement.*       filtrage, gradient, seuillage, conversion couleur→gris
                  caracteristiques.* histogrammes, descripteurs, distances, export CSV
                  image_io.*         chargement générique .pgm/.ppm, parcours de dossier
src/extraction.c programme principal de la Partie I : images/ → resultats/caracteristiques.csv
src/tp/         anciens TP conservés (tp1 étiquetage, tp3 Hough, exemple.c)
images/         images sources
resultats/      sorties générées (ignorées par git)
Makefile
```

## Compilation

```bash
make              # extraction, tp1, tp3
./extraction                                   # images/ → resultats/caracteristiques.csv
./extraction images resultats/caracteristiques.csv 100
```

Prérequis : `gcc` + `make` (MSYS2/MinGW ou WSL sous Windows).

## Partie I — caractéristiques calculées

Pour chaque image, une ligne de descripteur :

| Caractéristique | Rôle dans les requêtes |
|---|---|
| `largeur`, `hauteur`, `nb_pixels` | métadonnées |
| `est_couleur` | requête « images noir et blanc » |
| `moyenne_r/g/b`, `taux_r/g/b` | requête « peu de vert, beaucoup de rouge » |
| `saturation` | distinguer couleur / niveaux de gris |
| `luminance`, `contraste`, `entropie` | images sombres / claires / riches |
| `gradient_moyen`, `gradient_ecart_type` | requête « images texturées » |
| `nb_pixels_contour`, `taux_contour` | densité de contours |
| `hist_gris`, `hist_r`, `hist_g`, `hist_b` (16 classes) | distance entre images (VARRAY Oracle) |

Distances entre histogrammes implémentées : L1, L2, intersection, Bhattacharyya, χ².

## Partie II — Oracle

Serveur : `10.40.128.30:1521`, SID `emrepus`.
Interface Partie I → Partie II : le CSV `resultats/caracteristiques.csv`
(séparateur `;`), chargé par script PL/SQL, plus la signature `ordsys.ordimageSignature`
générée côté Oracle.
