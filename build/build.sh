#!/bin/bash
# ============================================================
# build.sh — Compile cubiomes → WASM
# ============================================================
# PRÉREQUIS :
#   1. Emscripten installé :
#        git clone https://github.com/emscripten-core/emsdk.git
#        cd emsdk && ./emsdk install latest && ./emsdk activate latest
#        source ./emsdk_env.sh
#
#   2. cubiomes cloné DANS ce dossier build/ :
#        git clone https://github.com/Cubitect/cubiomes cubiomes
#
# PUIS lancer ce script depuis le dossier build/ :
#        cd build && bash build.sh
# ============================================================

set -e

echo "=== MineGuide — Compilation cubiomes → WASM ==="

# Vérifie emcc
if ! command -v emcc &>/dev/null; then
  echo "ERREUR : emcc non trouvé. Active Emscripten d'abord :"
  echo "  source /path/to/emsdk/emsdk_env.sh"
  exit 1
fi

# Vérifie cubiomes
if [ ! -d "cubiomes" ]; then
  echo "Clone cubiomes..."
  git clone --depth=1 https://github.com/Cubitect/cubiomes cubiomes
fi

echo "Version cubiomes :"
head -1 cubiomes/README.md 2>/dev/null || echo "(inconnue)"

echo ""
echo "Compilation..."

emcc \
  cubiomes_wrapper.c \
  cubiomes/generator.c \
  cubiomes/biomes.c \
  cubiomes/layers.c \
  cubiomes/noise.c \
  cubiomes/util.c \
  cubiomes/finders.c \
  cubiomes/quadbase.c \
  -I cubiomes \
  -o ../assets/js/cubiomes.js \
  -s WASM=1 \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","HEAPU8","HEAP32","_malloc","_free"]' \
  -s EXPORTED_FUNCTIONS='["_malloc","_free","_cw_init","_cw_free","_cw_set_seed","_cw_get_biome","_cw_get_biome_bulk","_cw_get_structures"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INITIAL_MEMORY=67108864 \
  -s MAXIMUM_MEMORY=536870912 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="CubiomesModule" \
  -s ENVIRONMENT='worker' \
  -O2 \
  --no-entry

echo ""
echo "✅ Compilation réussie !"
echo "   Fichiers générés :"
echo "     ../assets/js/cubiomes.js"
echo "     ../assets/js/cubiomes.wasm"
echo ""
echo "Déplace-les si besoin, puis ouvre seedmap.html dans le navigateur."
