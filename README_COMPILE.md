# README — Compiler cubiomes en WASM pour MineGuide

## Prérequis

- **Git** installé
- **Python 3** installé (pour Emscripten)
- **CMake** (optionnel, pas nécessaire ici)
- ~2 Go d'espace disque pour Emscripten

---

## Étape 1 — Installer Emscripten (une seule fois)

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh   # à faire à chaque nouveau terminal
```

> **Windows** : remplacer `source ./emsdk_env.sh` par `emsdk_env.bat`

---

## Étape 2 — Cloner cubiomes

Depuis le dossier `build/` du projet MineGuide :

```bash
cd /chemin/vers/mineguide/build
git clone --depth=1 https://github.com/Cubitect/cubiomes cubiomes
```

---

## Étape 3 — Compiler

Toujours depuis `build/`, avec Emscripten activé :

```bash
source /chemin/vers/emsdk/emsdk_env.sh
bash build.sh
```

Durée : 20–60 secondes selon la machine.

---

## Résultat attendu

```
assets/js/cubiomes.js    ← Emscripten loader + glue
assets/js/cubiomes.wasm  ← Binaire WebAssembly (~400 Ko)
```

---

## Étape 4 — Tester

Ouvrir `seedmap.html` via un **serveur HTTP local** (obligatoire pour le WASM) :

```bash
# Python 3
cd /chemin/vers/mineguide
python3 -m http.server 8080
```

Puis ouvrir : http://localhost:8080/seedmap.html

> ⚠️ Ouvrir le fichier directement (`file://`) ne fonctionne pas avec les WASM+Workers.

---

## Problèmes courants

| Erreur | Solution |
|--------|----------|
| `emcc: command not found` | Relancer `source emsdk_env.sh` |
| `cubiomes/generator.h not found` | Vérifier que `build/cubiomes/` existe |
| Bannière rouge dans le navigateur | Vérifier que `.wasm` est dans `assets/js/` |
| CORS error dans la console | Utiliser `python3 -m http.server` |
