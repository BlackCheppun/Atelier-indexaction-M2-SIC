"""GUI minimal — Recherche par similarité Oracle (Vue SQL)."""

from __future__ import annotations

import time
import threading
import tkinter as tk
import re
from tkinter import messagebox, ttk

# --- Configuration Base de données ---
INSTANT_CLIENT = r"D:\Downloads\instantclient-basic-windows.x64-19.32.0.0.0dbru\instantclient_19_32"
USER = "tp_indexation"
PASSWORD = "123456"
DSN = "localhost:1522/orcl"
TABLE_NAME = "TP_INDEXATION.IMAGES"

# Noms de colonnes (à ajuster selon la base de données réelle)
COL_NOM = "nom"
COL_SIGNATURE = "signature"

COL_HISTO_R = "histo_r"
COL_HISTO_G = "histo_g"
COL_HISTO_B = "histo_b"
COL_HISTO_GRIS = "histo_gris"
COL_TAUX_R = "taux_r"
COL_TAUX_G = "taux_g"
COL_TAUX_B = "taux_b"
COL_DENSITE = "densite_contours"
COL_ISCOLOR = "is_color"
COL_LUMINOSITE = "luminosite_moyenne"
COL_SATURATION = "saturation_moyenne"


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
        self.var_dataset = tk.StringVar(value="toutes")
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
            "Histo Gris": tk.DoubleVar(value=0.0),
            "Taux R": tk.DoubleVar(value=0.0),
            "Taux G": tk.DoubleVar(value=0.0),
            "Taux B": tk.DoubleVar(value=0.0),
            "Densité Contours": tk.DoubleVar(value=0.0),
            "IsColor": tk.DoubleVar(value=0.0),
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
            dataset = self.var_dataset.get()
            where_clause = ""
            if dataset == "10":
                where_clause = "WHERE REGEXP_LIKE(nom, '^[a-zA-Z]+')"
            elif dataset == "500":
                where_clause = "WHERE REGEXP_LIKE(nom, '^[0-9]+')"

            self.cursor.execute(f"SELECT {COL_NOM} FROM {TABLE_NAME} {where_clause}")
            rows = self.cursor.fetchall()
            
            def natural_sort_key(s, _nsre=re.compile('([0-9]+)')):
                return [int(text) if text.isdigit() else text.lower() for text in _nsre.split(s[0])]
            
            rows.sort(key=natural_sort_key)
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

        # -1. Dataset
        frame_ds = ttk.LabelFrame(main_frame, text="Filtre sur la base d'images", padding=10)
        frame_ds.pack(fill="x", pady=(0, 10))
        ttk.Radiobutton(frame_ds, text="Toutes (510 images)", variable=self.var_dataset, value="toutes", command=self._fetch_images).pack(side="left", padx=10)
        ttk.Radiobutton(frame_ds, text="Les 10 images (Archives: arbres, bus...)", variable=self.var_dataset, value="10", command=self._fetch_images).pack(side="left", padx=10)
        ttk.Radiobutton(frame_ds, text="Les 500 images (Numérotées)", variable=self.var_dataset, value="500", command=self._fetch_images).pack(side="left", padx=10)

        # 0. Mode de recherche
        self.var_mode = tk.StringVar(value="compare")
        self.var_mode.trace_add("write", self._on_mode_change)
        
        frame_mode = ttk.LabelFrame(main_frame, text="Mode de recherche", padding=10)
        frame_mode.pack(fill="x", pady=(0, 10))
        ttk.Radiobutton(frame_mode, text="Recherche par similarité (Comparaison)", variable=self.var_mode, value="compare").pack(side="left", padx=10)
        ttk.Radiobutton(frame_mode, text="Recherche par critères (Filtrage global)", variable=self.var_mode, value="global").pack(side="left", padx=10)

        # 1. Sélection de l'image requête
        self.frame_req = ttk.LabelFrame(main_frame, text="Image de référence (Comparaison)", padding=10)
        self.frame_req.pack(fill="x", pady=(0, 10))
        
        ttk.Label(self.frame_req, text="Sélectionnez l'image cible :").pack(side="left", padx=(0, 10))
        self.cb_images = ttk.Combobox(self.frame_req, textvariable=self.var_image_req, state="readonly", width=40)
        self.cb_images.pack(side="left")

        # 2. Sliders (Poids)
        self.frame_sliders = ttk.LabelFrame(main_frame, text="Critères de recherche et pondérations", padding=10)
        self.frame_sliders.pack(fill="x", pady=(0, 10))
        
        self.frame_sliders.columnconfigure(0, weight=1)
        self.frame_sliders.columnconfigure(1, weight=1)

        # Sous-frame pour Oracle
        self.frame_ora = ttk.LabelFrame(self.frame_sliders, text="Reconnaissance globale (Oracle OrdImage)", padding=10)
        self._build_sliders(self.frame_ora, ["Oracle Couleur", "Oracle Texture", "Oracle Forme", "Oracle Localisation"])
        self.frame_ora.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)

        # Sous-frame pour Histogrammes
        self.frame_histos = ttk.LabelFrame(self.frame_sliders, text="Répartition des couleurs (Histogrammes)", padding=10)
        self._build_sliders(self.frame_histos, ["Histo R", "Histo G", "Histo B", "Histo Gris"])
        self.frame_histos.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

        # Sous-frame pour Taux RGB
        self.frame_taux = ttk.LabelFrame(self.frame_sliders, text="Prédominance des couleurs (Taux RGB)", padding=10)
        self._build_sliders(self.frame_taux, ["Taux R", "Taux G", "Taux B"])
        self.frame_taux.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
        self.frame_taux.grid_remove() # Caché par défaut car on commence en mode "compare"

        # Sous-frame Commune
        self.frame_common = ttk.LabelFrame(self.frame_sliders, text="Propriétés visuelles de l'image", padding=10)
        self._build_sliders(self.frame_common, ["Densité Contours", "IsColor", "Luminosité", "Saturation"])
        self.frame_common.grid(row=0, column=1, rowspan=2, sticky="nsew", padx=5, pady=5)

        # 3. Actions et Résultats
        frame_actions = ttk.Frame(main_frame)
        frame_actions.pack(fill="x", pady=(0, 10))
        
        ttk.Button(frame_actions, text="Rechercher", command=self._search).pack(side="left")
        
        # Frame pour le tableau et la scrollbar
        frame_tree = ttk.Frame(main_frame)
        frame_tree.pack(fill="both", expand=True)
        
        scrollbar = ttk.Scrollbar(frame_tree)
        scrollbar.pack(side="right", fill="y")
        
        self.tree = ttk.Treeview(frame_tree, columns=("rang", "nom", "score"), show="headings", yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.tree.yview)
        
        self.tree.heading("rang", text="Rang")
        self.tree.heading("nom", text="Image")
        self.tree.heading("score", text="Score (Distance / Différence)")
        self.tree.column("rang", width=60, anchor="center")
        self.tree.column("nom", width=300)
        self.tree.column("score", width=150)
        self.tree.pack(side="left", fill="both", expand=True)

    def _build_sliders(self, parent, labels):
        for idx, label in enumerate(labels):
            row = ttk.Frame(parent)
            row.pack(fill="x", pady=2)
            ttk.Label(row, text=label, width=18).pack(side="left")
            
            var = self.poids[label]
            scale = ttk.Scale(row, from_=0.0, to=1.0, variable=var, orient="horizontal")
            scale.pack(side="left", fill="x", expand=True, padx=5)
            
            val_lbl = ttk.Label(row, width=5)
            val_lbl.pack(side="left")
            # Callback pour màj de l'affichage de la valeur
            def update_lbl(v, l=val_lbl, var=var):
                l.config(text=f"{var.get():.2f}")
            scale.configure(command=update_lbl)
            update_lbl(None) # Init

    def _on_mode_change(self, *args):
        mode = self.var_mode.get()
        if mode == "global":
            self.frame_req.pack_forget()
            self.frame_ora.grid_remove()
            self.frame_histos.grid_remove()
            self.frame_taux.grid()
        else:
            self.frame_req.pack(fill="x", pady=(0, 10), before=self.frame_sliders)
            self.frame_taux.grid_remove()
            self.frame_ora.grid()
            self.frame_histos.grid()

    # ------------------------------------------------------------------ Recherche
    def _search(self) -> None:
        if not self.conn or not self.cursor:
            messagebox.showerror("Erreur", "Non connecté à la base de données.")
            return

        image_req = self.var_image_req.get()

        w_color = self.poids["Oracle Couleur"].get()
        w_texture = self.poids["Oracle Texture"].get()
        w_shape = self.poids["Oracle Forme"].get()
        w_loc = self.poids["Oracle Localisation"].get()

        w_hr = self.poids["Histo R"].get()
        w_hg = self.poids["Histo G"].get()
        w_hb = self.poids["Histo B"].get()
        w_hgris = self.poids["Histo Gris"].get()
        
        w_tr = self.poids["Taux R"].get()
        w_tg = self.poids["Taux G"].get()
        w_tb = self.poids["Taux B"].get()
        
        w_dens = self.poids["Densité Contours"].get()
        w_isc = self.poids["IsColor"].get()
        w_lum = self.poids["Luminosité"].get()
        w_sat = self.poids["Saturation"].get()

        # Construction de la requête SQL (Sélection directe)
        # On calcule une distance : 0 est identique, plus c'est grand moins c'est similaire.
        oracle_weights = (
            f'color="{w_color:.1f}",'
            f'texture="{w_texture:.1f}",'
            f'shape="{w_shape:.1f}",'
            f'location="{w_loc:.1f}"'
        )
        
        # NOTE: Si les colonnes maison n'existent pas encore dans la table, cette requête échouera.
        # Les valeurs absolues (ABS) mesurent la différence entre l'image requête (t1) et les autres (t2).
        dataset = self.var_dataset.get()
        filter_t2 = ""
        if dataset == "10":
            filter_t2 = "AND REGEXP_LIKE(t2.nom, '^[a-zA-Z]+')"
        elif dataset == "500":
            filter_t2 = "AND REGEXP_LIKE(t2.nom, '^[0-9]+')"

        mode = self.var_mode.get()

        if mode == "compare":
            if not image_req:
                messagebox.showwarning("Attention", "Veuillez sélectionner une image requête.")
                return

            if w_color > 0 or w_texture > 0 or w_shape > 0 or w_loc > 0:
                score_expr = f"ORDSYS.ORDImageSignature.evaluateScore(t1.{COL_SIGNATURE}, t2.{COL_SIGNATURE}, '{oracle_weights}')"
            else:
                score_expr = "0"
            
            if w_hr > 0:
                score_expr += f"\n                       + {w_hr} * bhattacharyya_distance(t1.{COL_HISTO_R}, t2.{COL_HISTO_R})"
            if w_hg > 0:
                score_expr += f"\n                       + {w_hg} * bhattacharyya_distance(t1.{COL_HISTO_G}, t2.{COL_HISTO_G})"
            if w_hb > 0:
                score_expr += f"\n                       + {w_hb} * bhattacharyya_distance(t1.{COL_HISTO_B}, t2.{COL_HISTO_B})"
            if w_hgris > 0:
                score_expr += f"\n                       + {w_hgris} * bhattacharyya_distance(t1.{COL_HISTO_GRIS}, t2.{COL_HISTO_GRIS})"
            
            if w_dens > 0:
                score_expr += f"\n                       + {w_dens} * ABS(NVL(t1.{COL_DENSITE},0) - NVL(t2.{COL_DENSITE},0))"
            if w_isc > 0:
                score_expr += f"\n                       + {w_isc} * ABS(NVL(t1.{COL_ISCOLOR},0) - NVL(t2.{COL_ISCOLOR},0))"
            if w_lum > 0:
                score_expr += f"\n                       + {w_lum} * ABS(NVL(t1.{COL_LUMINOSITE},0) - NVL(t2.{COL_LUMINOSITE},0))"
            if w_sat > 0:
                score_expr += f"\n                       + {w_sat} * ABS(NVL(t1.{COL_SATURATION},0) - NVL(t2.{COL_SATURATION},0))"

            sql_query = f"""
                SELECT t2.{COL_NOM} as NOM,
                       (
                           {score_expr}
                       ) AS SCORE
                FROM {TABLE_NAME} t1, {TABLE_NAME} t2
                WHERE t1.{COL_NOM} = '{image_req}' 
                  AND t2.{COL_NOM} != '{image_req}'
                  {filter_t2}
                ORDER BY SCORE ASC
            """
        else:
            # Mode "Global" : on n'a pas d'image de référence (t1 n'existe pas).
            # On cherche les images qui MAXIMISENT la somme pondérée des caractéristiques.
            # Plus le score est haut, plus l'image correspond aux critères choisis.
            score_expr = "0"
            if w_tr > 0:
                score_expr += f"\n                       + {w_tr} * NVL(t2.{COL_TAUX_R},0)"
            if w_tg > 0:
                score_expr += f"\n                       + {w_tg} * NVL(t2.{COL_TAUX_G},0)"
            if w_tb > 0:
                score_expr += f"\n                       + {w_tb} * NVL(t2.{COL_TAUX_B},0)"
            
            if w_dens > 0:
                score_expr += f"\n                       + {w_dens} * NVL(t2.{COL_DENSITE},0)"
            if w_isc > 0:
                score_expr += f"\n                       + {w_isc} * NVL(t2.{COL_ISCOLOR},0)"
            if w_lum > 0:
                score_expr += f"\n                       + {w_lum} * NVL(t2.{COL_LUMINOSITE},0)"
            if w_sat > 0:
                score_expr += f"\n                       + {w_sat} * NVL(t2.{COL_SATURATION},0)"
            
            where_global = filter_t2.replace("AND", "WHERE", 1) if filter_t2 else ""

            sql_query = f"""
                SELECT t2.{COL_NOM} as NOM,
                       (
                           {score_expr}
                       ) AS SCORE
                FROM {TABLE_NAME} t2
                {where_global}
                ORDER BY SCORE DESC
            """

        def work():
            try:
                self.after(0, lambda: self.status.set("Requête en cours..."))
                start_time = time.time()
                self.cursor.execute(sql_query)
                rows = self.cursor.fetchall()
                elapsed = time.time() - start_time
                
                self.after(0, lambda: self._fill_tree(rows))
                self.after(0, lambda: self.status.set(f"Recherche terminée en {elapsed:.6f}s ({len(rows)} résultats)."))
            except Exception as e:
                self.after(0, lambda: messagebox.showerror("Erreur SQL", str(e)))
                self.after(0, lambda: self.status.set("Erreur lors de la recherche."))

        threading.Thread(target=work, daemon=True).start()

    def _fill_tree(self, rows):
        # Nettoyage
        for item in self.tree.get_children():
            self.tree.delete(item)
        # Remplissage
        for i, r in enumerate(rows, 1):
            # r[0] = Nom, r[1] = Score
            score_formatted = f"{r[1]:.8f}" if r[1] is not None else "N/A"
            self.tree.insert("", "end", values=(f"#{i}", r[0], score_formatted))


if __name__ == "__main__":
    App().mainloop()
