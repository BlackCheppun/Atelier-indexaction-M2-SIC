# ============================================================================
#  Atelier Indexation d'images - M2 IISC Pro
#  Partie I : extraction des caracteristiques images en C
#  ---------------------------------------------------------------------------
#  Toutes les commandes se lancent DEPUIS LA RACINE du projet :
#     make            -> compile extraction, tp1, tp3
#     make extraction -> le programme de l'atelier
#     ./extraction    -> lit images/  et ecrit resultats/caracteristiques.csv
#     make clean
# ============================================================================

CC       = gcc
CFLAGS   = -Wall -Wextra -O2 -std=c99 -INRC -Isrc/lib
LDLIBS   = -lm

# Bibliotheque NRC (fournie, NE PAS MODIFIER)
NRC_SRC  = NRC/nrio.c NRC/nralloc.c NRC/nrarith.c

# Notre bibliotheque (code reutilisable de l'equipe)
LIB_SRC  = src/lib/traitement.c src/lib/caracteristiques.c src/lib/image_io.c

BINAIRES = extraction tp1 tp3 test_couleur test_luminance test_gris

all: $(BINAIRES)

extraction: src/extraction.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

tp1: src/tp/tp1.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

tp3: src/tp/tp3.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

test_couleur: src/tests/test_couleur.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

test_luminance: src/tests/test_luminance.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

test_gris: src/tests/test_gris.c $(LIB_SRC) $(NRC_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(BINAIRES) $(addsuffix .exe,$(BINAIRES))

mrproper: clean
	rm -f resultats/*.pgm resultats/*.ppm resultats/*.csv resultats/*.txt

.PHONY: all clean mrproper
