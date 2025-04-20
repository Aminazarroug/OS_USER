# Sherlock 13 - Projet OS USER

Bienvenue dans notre adaptation en C du jeu **Sherlock 13**, réalisée dans le cadre du module OS USER dans le cadre des études en ingénierie "EI4" à Polytech Sorbonne. Ce projet combine sockets, threads, synchronisation, et interface graphique avec SDL2 pour permettre à 4 joueurs de mîner une enquête en réseau ! 📝🚀

---

## 🔧 Compilation

Avant tout, assurez-vous d'avoir les dépendances suivantes installées sur votre système :

- `gcc`
- `make`
- `libsdl2-dev`
- `libsdl2-image-dev`
- `libsdl2-ttf-dev`
- (sous Debian/Ubuntu : `sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev`)

Ensuite, il suffit de lancer :

```bash
make
```

Cela génère deux exécutables :
- `server`
- `sh13`

---

## 🏠 Lancement d'une partie (en local ou en LAN)

### 1. Lancer le serveur :

```bash
./server 4000
```

(`4000` est le port, vous pouvez en choisir un autre)

### 2. Lancer chaque client (dans 4 terminaux ou sur 4 machines) :

```bash
./sh13 <IP serveur> <Port serveur> <IP client> <Port client> <Nom>
```

Exemple en local :
```bash
./sh13 127.0.0.1 4000 127.0.0.1 5001 Alice
./sh13 127.0.0.1 4000 127.0.0.1 5002 Bob
./sh13 127.0.0.1 4000 127.0.0.1 5003 Charlie
./sh13 127.0.0.1 4000 127.0.0.1 5004 Diana
```
Important : Il faut bien exécuter tout ça dans le dossier src !
Quand les 4 joueurs sont connectés, la partie commence automatiquement !

---

## 🎨 Interface et commandes

- **Go** : pour valider une action (interrogation ou accusation).
- **Connect** : pour rejoindre le serveur.
- **Replay** : pour relancer une partie.
- **Emojis** : petits messages visuels entre joueurs.

L'interface SDL affiche les cartes, les symboles, les noms des joueurs, et permet de cliquer pour jouer (aucun input clavier requis).

---

## 🎓 Fonctionnalités techniques clés

- Communication client/serveur en TCP avec `socket()`
- Thread d'écoute réseau pour chaque client
- Synchronisation via `volatile` et mutex
- Interface graphique interactive avec SDL2
- Protocole de messages simple mais efficace

---

## 📖 Rapport

Le fichier `Final_rapport.pdf` décrit en détail :
- Notre approche de développement
- Le fonctionnement du jeu
- Les choix techniques
- L'utilisation des notions du module (sockets, threads, etc.)

---
Amina ZARROUG  ~ je vous souhaite un bon jeu ⚡️🤖

