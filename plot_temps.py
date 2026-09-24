import matplotlib.pyplot as plt
import numpy as np

# Données extraites
categories = ['Oracle (Signature)', 'Custom (Base)', '+1 Histogramme', '+2 Histogrammes', '+3 Histogrammes', '+4 Histogrammes']

temps_10 = [0.043, 0.01, 0.03, 0.04, 0.05, 0.06]
temps_510 = [0.597, 0.02, 0.46, 0.99, 1.40, 1.80]

x = np.arange(len(categories))
width = 0.35

fig, ax = plt.subplots(figsize=(10, 6))
rects1 = ax.bar(x - width/2, temps_10, width, label='10 images', color='#4CAF50')
rects2 = ax.bar(x + width/2, temps_510, width, label='510 images', color='#2196F3')

# Ajout de textes, titre et étiquettes
ax.set_ylabel('Temps d\'exécution (secondes)')
ax.set_title('Comparatif des temps d\'exécution de la recherche selon la méthode et le nombre d\'images')
ax.set_xticks(x)
ax.set_xticklabels(categories, rotation=45, ha='right')
ax.legend()

# Ajouter les valeurs sur les barres
def autolabel(rects):
    for rect in rects:
        height = rect.get_height()
        ax.annotate(f'{height}',
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points offset
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=9)

autolabel(rects1)
autolabel(rects2)

plt.tight_layout()
plt.savefig('graphe_temps_execution.png', dpi=300)
print('Graphique généré avec succès dans graphe_temps_execution.png')
