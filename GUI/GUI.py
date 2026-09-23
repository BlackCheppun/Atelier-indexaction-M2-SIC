"""GUI minimal — Recherche par similarité Oracle (Vue SQL)."""

from __future__ import annotations

import threading
import tkinter as tk
from tkinter import messagebox, ttk

# --- Configuration Base de données ---
INSTANT_CLIENT = r"D:\Downloads\instantclient-basic-windows.x64-19.32.0.0.0dbru\instantclient_19_32"
USER = "tp_indexation"
PASSWORD = "123456"
DSN = "localhost:1522/orcl"
TABLE_NAME = "TP_INDEXATION.TEST_MULTIMEDIA"

# Noms de colonnes (à ajuster selon la base de données réelle)
COL_NOM = "NOM"
COL_SIGNATURE = "SIGNATURE"

COL_HISTO_R = "HISTO_R"
COL_HISTO_G = "HISTO_G"
COL_HISTO_B = "HISTO_B"
COL_DENSITE = "DENSITE_CONTOURS"
COL_ISCOLOR = "IS_COLOR"
COL_TEXTURE = "TEXTURE"
COL_LUMINOSITE = "LUMINOSITE"
COL_SATURATION = "SATURATION"


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Recherche d'images - Comparaisons")
        self.geometry("900x700")
        self.minsize(800, 600)

        self.conn = None
        self.cursor = None

        self.status = tk.StringVar(value="Connexion à Oracle en cours…")
        ttk.Label(self, textvariable=self.status, relief="sunken", anchor="w").pack(side="bottom", fill="x")

        # Variables pour la sélection
        self.var_image_req = tk.StringVar()
        self.images_list = []

        # Variables des poids (Sliders)
        self.poids = {
            "Oracle Couleur": tk.DoubleVar(value=0.25),
            "Oracle Texture": tk.DoubleVar(value=0.25),
            "Oracle Forme": tk.DoubleVar(value=0.25),
            "Oracle Localisation": tk.DoubleVar(value=0.25),
            "Histo R": tk.DoubleVar(value=0.0),
            "Histo G": tk.DoubleVar(value=0.0),
            "Histo B": tk.DoubleVar(value=0.0),
            "Densité Contours": tk.DoubleVar(value=0.0),
            "IsColor": tk.DoubleVar(value=0.0),
            "Texture Maison": tk.DoubleVar(value=0.0),
            "Luminosité": tk.DoubleVar(value=0.0),
            "Saturation": tk.DoubleVar(value=0.0),
        }

        self._build_ui()
        threading.Thread(target=self._connect, daemon=True).start()

    # ------------------------------------------------------------------ connexion
    def _connect(self) -> None:
        try:
            import oracledb

            oracledb.init_oracle_client(lib_dir=INSTANT_CLIENT)
            self.conn = oracledb.connect(user=USER, password=PASSWORD, dsn=DSN)
            self.cursor = self.conn.cursor()
            self.after(0, lambda: self.status.set(f"Connecté — {USER}@{DSN}"))
            self._fetch_images()
        except Exception as e:
            self.after(0, lambda: self.status.set(f"Hors ligne — {e}"))

    def _fetch_images(self) -> None:
        try:
            self.cursor.execute(f"SELECT {COL_NOM} FROM {TABLE_NAME} ORDER BY {COL_NOM}")
            rows = self.cursor.fetchall()
            self.images_list = [r[0] for r in rows]
            self.after(0, self._update_cb_images)
        except Exception as e:
            print("Erreur fetch images:", e)

    def _update_cb_images(self):
        self.cb_images['values'] = self.images_list
        if self.images_list:
            self.cb_images.current(0)

    # ------------------------------------------------------------------ Interface
    def _build_ui(self) -> None:
        main_frame = ttk.Frame(self, padding=10)
        main_frame.pack(fill="both", expand=True)

        # 1. Sélection de l'image requête
        frame_req = ttk.LabelFrame(main_frame, text="1. Image Requête (depuis Oracle)", padding=10)
        frame_req.pack(fill="x", pady=(0, 10))

        ttk.Label(frame_req, text="Sélectionnez l'image :").pack(side="left", padx=(0, 10))
        self.cb_images = ttk.Combobox(frame_req, textvariable=self.var_image_req, state="readonly", width=40)
        self.cb_images.pack(side="left")

        # 2. Sliders (Poids)
        frame_sliders = ttk.LabelFrame(main_frame, text="2. Pondérations des caractéristiques", padding=10)
        frame_sliders.pack(fill="x", pady=(0, 10))

        # Sous-frame pour Oracle
        frame_ora = ttk.LabelFrame(frame_sliders, text="Signatures natives Oracle (OrdImage)", padding=10)
        frame_ora.pack(side="left", fill="both", expand=True, padx=(0, 5))
        self._build_sliders(frame_ora, ["Oracle Couleur", "Oracle Texture", "Oracle Forme", "Oracle Localisation"])

        # Sous-frame pour Maison
        frame_maison = ttk.LabelFrame(frame_sliders, text="Caractéristiques extraites (Maison)", padding=10)
        frame_maison.pack(side="left", fill="both", expand=True, padx=(5, 0))
        self._build_sliders(frame_maison, [
            "Histo R", "Histo G", "Histo B", "Densité Contours",
            "IsColor", "Texture Maison", "Luminosité", "Saturation"
        ])

        # 3. Actions et Résultats
        frame_actions = ttk.Frame(main_frame)
        frame_actions.pack(fill="x", pady=(0, 10))
        
        ttk.Button(frame_actions, text="Créer Vue & Rechercher", command=self._search).pack(side="left")
        
        self.tree = ttk.Treeview(main_frame, columns=("nom", "score"), show="headings")
        self.tree.heading("nom", text="Image")
        self.tree.heading("score", text="Score (Distance / Différence)")
        self.tree.column("nom", width=300)
        self.tree.column("score", width=150)
        self.tree.pack(fill="both", expand=True)

    def _build_sliders(self, parent, labels):
        for idx, label in enumerate(labels):
            row = ttk.Frame(parent)
            row.pack(fill="x", pady=2)
            ttk.Label(row, text=label, width=18).pack(side="left")
            
            var = self.poids[label]
            scale = ttk.Scale(row, from_=0.0, to=10.0, variable=var, orient="horizontal")
            scale.pack(side="left", fill="x", expand=True, padx=5)
            
            val_lbl = ttk.Label(row, width=5)
            val_lbl.pack(side="left")
            # Callback pour màj de l'affichage de la valeur
            def update_lbl(v, l=val_lbl, var=var):
                l.config(text=f"{var.get():.1f}")
            scale.configure(command=update_lbl)
            update_lbl(None) # Init

    # ------------------------------------------------------------------ Recherche
    def _search(self) -> None:
        if not self.conn or not self.cursor:
            messagebox.showerror("Erreur", "Non connecté à la base de données.")
            return

        image_req = self.var_image_req.get()
        if not image_req:
            messagebox.showwarning("Attention", "Veuillez sélectionner une image requête.")
            return

        w_color = self.poids["Oracle Couleur"].get()
        w_texture = self.poids["Oracle Texture"].get()
        w_shape = self.poids["Oracle Forme"].get()
        w_loc = self.poids["Oracle Localisation"].get()

        w_hr = self.poids["Histo R"].get()
        w_hg = self.poids["Histo G"].get()
        w_hb = self.poids["Histo B"].get()
        w_dens = self.poids["Densité Contours"].get()
        w_isc = self.poids["IsColor"].get()
        w_texm = self.poids["Texture Maison"].get()
        w_lum = self.poids["Luminosité"].get()
        w_sat = self.poids["Saturation"].get()

        # Construction de la requête SQL (Création de la vue)
        # On calcule une distance : 0 est identique, plus c'est grand moins c'est similaire.
        oracle_weights = f"color={w_color} texture={w_texture} shape={w_shape} location={w_loc}"
        
        # NOTE: Si les colonnes maison n'existent pas encore dans la table, cette requête échouera.
        # Les valeurs absolues (ABS) mesurent la différence entre l'image requête (t1) et les autres (t2).
        sql_create_view = f"""
            CREATE OR REPLACE VIEW VUE_COMPARAISON AS
            SELECT t2.{COL_NOM} as NOM,
                   (
                       -- Score Oracle (retourne une distance)
                       ORDSYS.SI_Score(t1.{COL_SIGNATURE}, t2.{COL_SIGNATURE}, '{oracle_weights}')
                       
                       -- Ajout des différences sur les caractéristiques maison pondérées
                       -- Décommentez/Ajustez ces lignes lorsque les colonnes existent
                       /*
                       + {w_hr} * ABS(NVL(t1.{COL_HISTO_R},0) - NVL(t2.{COL_HISTO_R},0))
                       + {w_hg} * ABS(NVL(t1.{COL_HISTO_G},0) - NVL(t2.{COL_HISTO_G},0))
                       + {w_hb} * ABS(NVL(t1.{COL_HISTO_B},0) - NVL(t2.{COL_HISTO_B},0))
                       + {w_dens} * ABS(NVL(t1.{COL_DENSITE},0) - NVL(t2.{COL_DENSITE},0))
                       + {w_isc} * ABS(NVL(t1.{COL_ISCOLOR},0) - NVL(t2.{COL_ISCOLOR},0))
                       + {w_texm} * ABS(NVL(t1.{COL_TEXTURE},0) - NVL(t2.{COL_TEXTURE},0))
                       + {w_lum} * ABS(NVL(t1.{COL_LUMINOSITE},0) - NVL(t2.{COL_LUMINOSITE},0))
                       + {w_sat} * ABS(NVL(t1.{COL_SATURATION},0) - NVL(t2.{COL_SATURATION},0))
                       */
                   ) AS SCORE
            FROM {TABLE_NAME} t1, {TABLE_NAME} t2
            WHERE t1.{COL_NOM} = '{image_req}' 
              AND t2.{COL_NOM} != '{image_req}'
        """

        def work():
            try:
                self.after(0, lambda: self.status.set("Création de la vue en cours..."))
                # Création de la vue
                self.cursor.execute(sql_create_view)
                
                # Sélection depuis la vue
                self.after(0, lambda: self.status.set("Requête sur la vue en cours..."))
                self.cursor.execute("SELECT NOM, SCORE FROM VUE_COMPARAISON ORDER BY SCORE ASC")
                rows = self.cursor.fetchall()
                
                self.after(0, lambda: self._fill_tree(rows))
                self.after(0, lambda: self.status.set(f"Recherche terminée ({len(rows)} résultats)."))
            except Exception as e:
                self.after(0, lambda: messagebox.showerror("Erreur SQL", str(e)))
                self.after(0, lambda: self.status.set("Erreur lors de la recherche."))

        threading.Thread(target=work, daemon=True).start()

    def _fill_tree(self, rows):
        # Nettoyage
        for item in self.tree.get_children():
            self.tree.delete(item)
        # Remplissage
        for r in rows:
            # r[0] = Nom, r[1] = Score
            score_formatted = f"{r[1]:.4f}" if r[1] is not None else "N/A"
            self.tree.insert("", "end", values=(r[0], score_formatted))


if __name__ == "__main__":
    App().mainloop()
