"""GUI minimal — indexation C→Oracle et recherche par similarité."""

from __future__ import annotations

import os
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

INSTANT_CLIENT = r"D:\Downloads\instantclient-basic-windows.x64-19.32.0.0.0dbru\instantclient_19_32"
USER = "tp_indexation"
PASSWORD = "123456"
DSN = "localhost:1522/orcl"
ORACLE_DIR = "IMG10"

REPO = Path(__file__).resolve().parents[1]


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Indexation d'images")
        self.geometry("780x520")
        self.minsize(700, 460)

        self.conn = None
        self.query_path = ""
        self.thumbs: list = []

        self.status = tk.StringVar(value="Connexion…")
        ttk.Label(self, textvariable=self.status).pack(anchor="w", padx=10, pady=(8, 0))

        nb = ttk.Notebook(self)
        nb.pack(fill="both", expand=True, padx=10, pady=8)

        self.page_load = ttk.Frame(nb, padding=8)
        self.page_search = ttk.Frame(nb, padding=8)
        nb.add(self.page_load, text="Indexation")
        nb.add(self.page_search, text="Recherche")

        self._build_load()
        self._build_search()
        threading.Thread(target=self._connect, daemon=True).start()

    # ------------------------------------------------------------------ connexion
    def _connect(self) -> None:
        try:
            import oracledb

            oracledb.init_oracle_client(lib_dir=INSTANT_CLIENT)
            self.conn = oracledb.connect(user=USER, password=PASSWORD, dsn=DSN)
            self.after(0, lambda: self.status.set(f"Connecté — {USER}@{DSN}"))
        except Exception as e:  # noqa: BLE001
            self.after(0, lambda: self.status.set(f"Hors ligne — {e}"))

    # ------------------------------------------------------------------ indexation
    def _build_load(self) -> None:
        row = ttk.Frame(self.page_load)
        row.pack(fill="x")
        ttk.Label(row, text="Dossier images").pack(side="left")
        self.var_dir = tk.StringVar(value=str(REPO / "images"))
        ttk.Entry(row, textvariable=self.var_dir).pack(side="left", fill="x", expand=True, padx=6)
        ttk.Button(row, text="…", width=3, command=self._pick_dir).pack(side="left")

        ttk.Button(self.page_load, text="Indexer (C + Oracle)", command=self._indexer).pack(
            anchor="w", pady=8
        )

        self.log = tk.Text(self.page_load, height=18, wrap="word")
        self.log.pack(fill="both", expand=True)
        self._log("Un bouton : extraction C puis import Oracle (images, signatures, descripteurs).")

    def _pick_dir(self) -> None:
        p = filedialog.askdirectory(initialdir=self.var_dir.get() or ".")
        if p:
            self.var_dir.set(p)

    def _log(self, msg: str) -> None:
        self.log.insert("end", msg.rstrip() + "\n")
        self.log.see("end")

    def _indexer(self) -> None:
        dossier = self.var_dir.get()

        def work():
            self.after(0, lambda: self._log(f"Dossier : {dossier}"))
            self.after(0, lambda: self._log(f"DIRECTORY Oracle : {ORACLE_DIR}"))
            # TODO : lancer le binaire C, puis importFrom / generateSignature / CSV
            self.after(0, lambda: self._log("(stub) extraction C → CSV"))
            self.after(0, lambda: self._log("(stub) import images + signatures"))
            self.after(0, lambda: self._log("(stub) chargement des descripteurs"))
            self.after(0, lambda: self._log("Terminé."))

        threading.Thread(target=work, daemon=True).start()

    # ------------------------------------------------------------------ recherche
    def _build_search(self) -> None:
        row = ttk.Frame(self.page_search)
        row.pack(fill="x")
        ttk.Button(row, text="Image requête…", command=self._pick_query).pack(side="left")
        self.lbl_query = ttk.Label(row, text="aucune")
        self.lbl_query.pack(side="left", padx=8)

        opts = ttk.Frame(self.page_search)
        opts.pack(fill="x", pady=8)
        self.var_method = tk.StringVar(value="both")
        ttk.Radiobutton(opts, text="Oracle", value="oracle", variable=self.var_method).pack(side="left")
        ttk.Radiobutton(opts, text="Maison", value="maison", variable=self.var_method).pack(side="left", padx=8)
        ttk.Radiobutton(opts, text="Les deux", value="both", variable=self.var_method).pack(side="left")
        ttk.Label(opts, text="Top").pack(side="left", padx=(16, 4))
        self.var_k = tk.IntVar(value=5)
        ttk.Spinbox(opts, from_=1, to=20, textvariable=self.var_k, width=4).pack(side="left")
        ttk.Button(opts, text="Rechercher", command=self._search).pack(side="left", padx=12)

        filtres = ttk.Frame(self.page_search)
        filtres.pack(fill="x")
        ttk.Button(filtres, text="Peu de vert, beaucoup de rouge", command=lambda: self._filtre("rouge")).pack(
            side="left"
        )
        ttk.Button(filtres, text="N&B", command=lambda: self._filtre("nb")).pack(side="left", padx=4)
        ttk.Button(filtres, text="Texturées", command=lambda: self._filtre("tex")).pack(side="left")

        self.lbl_time = ttk.Label(self.page_search, text="")
        self.lbl_time.pack(anchor="w", pady=(8, 2))

        self.tree = ttk.Treeview(self.page_search, columns=("source", "nom", "score"), show="headings")
        self.tree.heading("source", text="source")
        self.tree.heading("nom", text="image")
        self.tree.heading("score", text="score")
        self.tree.column("source", width=140)
        self.tree.column("nom", width=280)
        self.tree.column("score", width=80)
        self.tree.pack(fill="both", expand=True, pady=(4, 0))

    def _pick_query(self) -> None:
        p = filedialog.askopenfilename(
            filetypes=[("Images", "*.jpg *.jpeg *.png *.ppm *.pgm *.gif"), ("Tous", "*.*")]
        )
        if p:
            self.query_path = p
            self.lbl_query.configure(text=os.path.basename(p))

    def _fill(self, rows: list[tuple[str, str, str]], timing: str = "") -> None:
        self.tree.delete(*self.tree.get_children())
        for r in rows:
            self.tree.insert("", "end", values=r)
        self.lbl_time.configure(text=timing)

    def _search(self) -> None:
        if not self.query_path:
            messagebox.showinfo("Recherche", "Choisissez une image.")
            return
        method = self.var_method.get()
        k = int(self.var_k.get())

        def work():
            rows = []
            times = []
            if method in ("oracle", "both"):
                rows += [("Oracle", "img01.jpg", "92.4"), ("Oracle", "img04.jpg", "81.1")][:k]
                times.append("Oracle 12 ms")
            if method in ("maison", "both"):
                rows += [("Maison", "img01.jpg", "0.12"), ("Maison", "img02.jpg", "0.19")][:k]
                times.append("Maison 8 ms")
            self.after(0, lambda: self._fill(rows, "  |  ".join(times)))

        threading.Thread(target=work, daemon=True).start()

    def _filtre(self, kind: str) -> None:
        data = {
            "rouge": [("Filtre", "rouge_01.jpg", "taux_r élevé")],
            "nb": [("Filtre", "scan.pgm", "N&B")],
            "tex": [("Filtre", "tex_01.jpg", "gradient élevé")],
        }
        self._fill(data.get(kind, []), "requête sur caractéristiques maison")


if __name__ == "__main__":
    App().mainloop()
