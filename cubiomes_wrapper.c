/**
 * cubiomes_worker.js
 * Web Worker — charge cubiomes.wasm et traite les requêtes de tuiles/structures.
 * Tout le calcul lourd se fait ici, le thread principal reste fluide.
 */
"use strict";

/* ────────────────────────────────────────────────────────────
   COULEURS BIOMES (identiques à Minecraft 1.21)
   Source : https://minecraft.wiki/w/Biome + wiki.vg
──────────────────────────────────────────────────────────── */
const BIOME_COLORS = [
/* 0  ocean              */ [0,0,112],
/* 1  plains             */ [141,179,96],
/* 2  desert             */ [250,148,24],
/* 3  mountains/windswept*/ [96,96,96],
/* 4  forest             */ [5,102,33],
/* 5  taiga              */ [11,102,89],
/* 6  swamp              */ [7,249,178],
/* 7  river              */ [0,0,255],
/* 8  nether_wastes      */ [191,59,59],
/* 9  the_end            */ [128,128,255],
/* 10 frozen_ocean       */ [144,144,160],
/* 11 frozen_river       */ [160,160,255],
/* 12 snowy_plains       */ [255,255,255],
/* 13 snowy_mountains    */ [160,160,160],
/* 14 mushroom_fields    */ [255,0,255],
/* 15 mushroom_field_shore*/[160,0,255],
/* 16 beach              */ [250,222,85],
/* 17 desert_hills       */ [210,95,18],
/* 18 wooded_hills       */ [34,85,28],
/* 19 taiga_hills        */ [22,57,51],
/* 20 mountain_edge      */ [114,120,154],
/* 21 jungle             */ [83,123,9],
/* 22 jungle_hills       */ [44,66,5],
/* 23 sparse_jungle      */ [99,166,47],
/* 24 deep_ocean         */ [0,0,48],
/* 25 stone_shore        */ [162,162,132],
/* 26 snowy_beach        */ [250,240,192],
/* 27 birch_forest       */ [48,116,68],
/* 28 birch_forest_hills */ [31,95,50],
/* 29 dark_forest        */ [64,81,26],
/* 30 snowy_taiga        */ [49,85,74],
/* 31 snowy_taiga_hills  */ [36,63,54],
/* 32 old_growth_pine_taiga*/[89,102,81],
/* 33 old_growth_spruce_taiga*/[69,79,62],
/* 34 wooded_mountains   */ [80,112,80],
/* 35 savanna            */ [189,178,95],
/* 36 savanna_plateau    */ [167,157,100],
/* 37 badlands           */ [217,69,21],
/* 38 eroded_badlands    */ [255,109,61],
/* 39 wooded_badlands    */ [176,151,101],
/* 40 [reserved]         */ [100,100,100],
/* 41 [reserved]         */ [100,100,100],
/* 42 [reserved]         */ [100,100,100],
/* 43 [reserved]         */ [100,100,100],
/* 44 [reserved]         */ [100,100,100],
/* 45 [reserved]         */ [100,100,100],
/* 46 [reserved]         */ [100,100,100],
/* 47 [reserved]         */ [100,100,100],
/* 48 [reserved]         */ [100,100,100],
/* 49 [reserved]         */ [100,100,100],
/* 50 [reserved]         */ [100,100,100],
/* 51 [reserved]         */ [100,100,100],
/* 52 [reserved]         */ [100,100,100],
/* 53 [reserved]         */ [100,100,100],
/* 54 [reserved]         */ [100,100,100],
/* 55 [reserved]         */ [100,100,100],
/* 56 [reserved]         */ [100,100,100],
/* 57 [reserved]         */ [100,100,100],
/* 58 [reserved]         */ [100,100,100],
/* 59 [reserved]         */ [100,100,100],
/* 60 snowy_plains       */ [255,255,255],
/* 61 ice_spikes         */ [180,220,220],
/* 62 [reserved]         */ [100,100,100],
/* 63 [reserved]         */ [100,100,100],
/* 64 [reserved]         */ [100,100,100],
/* 65 [reserved]         */ [100,100,100],
/* 66 [reserved]         */ [100,100,100],
/* 67 [reserved]         */ [100,100,100],
/* 68 [reserved]         */ [100,100,100],
/* 69 [reserved]         */ [100,100,100],
/* 70 [reserved]         */ [100,100,100],
/* 71 [reserved]         */ [100,100,100],
/* 72 [reserved]         */ [100,100,100],
/* 73 [reserved]         */ [100,100,100],
/* 74 [reserved]         */ [100,100,100],
/* 75 [reserved]         */ [100,100,100],
/* 76 [reserved]         */ [100,100,100],
/* 77 [reserved]         */ [100,100,100],
/* 78 [reserved]         */ [100,100,100],
/* 79 [reserved]         */ [100,100,100],
/* 80 [reserved]         */ [100,100,100],
/* 81 [reserved]         */ [100,100,100],
/* 82 [reserved]         */ [100,100,100],
/* 83 [reserved]         */ [100,100,100],
/* 84 [reserved]         */ [100,100,100],
/* 85 soul_sand_valley   */ [93,68,32],
/* 86 crimson_forest     */ [221,8,8],
/* 87 warped_forest      */ [73,144,123],
/* 88 basalt_deltas      */ [84,84,94],
/* 89 [reserved]         */ [100,100,100],
/* 90 [reserved]         */ [100,100,100],
/* 91 [reserved]         */ [100,100,100],
/* 92 [reserved]         */ [100,100,100],
/* 93 [reserved]         */ [100,100,100],
/* 94 [reserved]         */ [100,100,100],
/* 95 [reserved]         */ [100,100,100],
/* 96 [reserved]         */ [100,100,100],
/* 97 [reserved]         */ [100,100,100],
/* 98 [reserved]         */ [100,100,100],
/* 99 [reserved]         */ [100,100,100],
/* 100 [reserved]        */ [100,100,100],
/* 101-177 : réservés    */
];
/* Étend la table pour couvrir tous les ids jusqu'à 200 */
while (BIOME_COLORS.length < 201) BIOME_COLORS.push([100,100,100]);

/* Biomes 1.18+ qui ont des IDs plus grands dans cubiomes */
const BIOME_COLORS_EXT = {
  /* Meadow */ 177: [83,179,96],
  /* Grove  */ 178: [200,220,255],
  /* Snowy_slopes */ 179: [205,210,220],
  /* Frozen_peaks */ 180: [160,180,200],
  /* Jagged_peaks */ 181: [200,200,200],
  /* Stony_peaks  */ 182: [210,200,170],
  /* Cherry_grove */ 183: [255,183,197],
  /* Deep_dark    */ 184: [15,15,25],
  /* Mangrove_swamp */ 185: [45,120,60],
  /* Pale_garden */ 186: [220,230,215],
};

function biomeColor(id) {
  if (id in BIOME_COLORS_EXT) return BIOME_COLORS_EXT[id];
  if (id >= 0 && id < BIOME_COLORS.length) return BIOME_COLORS[id];
  return [100,100,100];
}

/* ────────────────────────────────────────────────────────────
   ÉTAT DU WORKER
──────────────────────────────────────────────────────────── */
let Module   = null; // module WASM chargé
let cwGen    = 0;    // pointeur CWGen*
let isReady  = false;

/* Buffer partagé pour cw_get_biome_bulk */
let biomeBuf    = null; // pointeur WASM (int32)
let biomeBufLen = 0;    // en nombre d'ints

/* ────────────────────────────────────────────────────────────
   CHARGEMENT DU MODULE WASM
──────────────────────────────────────────────────────────── */
importScripts('cubiomes.js'); // Emscripten output

CubiomesModule().then(function(m) {
  Module = m;
  isReady = true;
  postMessage({ type: 'ready' });
}).catch(function(e) {
  postMessage({ type: 'error', msg: 'Impossible de charger cubiomes.wasm : ' + e.message });
});

/* ────────────────────────────────────────────────────────────
   VERSION STRING → INT (pour le wrapper C)
   ex: "1.21.4" → 1214, "1.18" → 1180, "1.16" → 1160
──────────────────────────────────────────────────────────── */
function versionToInt(v) {
  const parts = String(v).split('.').map(Number);
  const major = parts[1] || 16;
  const minor = parts[2] || 0;
  return 1000 + major * 10 + Math.min(minor, 9);
}

/* ────────────────────────────────────────────────────────────
   SEED → LO/HI (int32 bas/haut pour uint64)
──────────────────────────────────────────────────────────── */
function seedParts(seed) {
  /* seed est un Number JS (peut être négatif, jusqu'à ±2^53) */
  seed = Math.trunc(seed);
  const lo = seed | 0;                         // 32 bits bas
  const hi = Math.trunc(seed / 0x100000000);   // 32 bits hauts
  return [lo, hi];
}

/* ────────────────────────────────────────────────────────────
   INITIALISE LE GÉNÉRATEUR
──────────────────────────────────────────────────────────── */
function initGen(seed, version, largeBiomes) {
  if (!Module) return false;

  /* Libère l'ancien générateur */
  if (cwGen !== 0) {
    Module._cw_free(cwGen);
    cwGen = 0;
  }

  const vint = versionToInt(version);
  cwGen = Module._cw_init(vint, largeBiomes ? 1 : 0);
  if (!cwGen) return false;

  const [lo, hi] = seedParts(seed);
  Module._cw_set_seed(cwGen, lo, hi);
  return true;
}

/* ────────────────────────────────────────────────────────────
   GÉNÈRE UNE TUILE BIOME → Uint8ClampedArray RGBA
   tileX, tileZ : coin supérieur-gauche en blocs
   tileSz       : taille de la tuile en blocs
   px           : taille en pixels de la tuile (ex: 64)
──────────────────────────────────────────────────────────── */
function generateTile(tileX, tileZ, tileSz, px) {
  if (!Module || !cwGen) return null;

  /* On utilise cw_get_biome_bulk avec scale = tileSz/px */
  const scale = Math.max(1, Math.round(tileSz / px));

  /* S'assure que le buffer WASM est assez grand */
  const needed = px * px;
  if (!biomeBuf || biomeBufLen < needed) {
    if (biomeBuf) Module._free(biomeBuf);
    biomeBuf    = Module._malloc(needed * 4); // int32 = 4 octets
    biomeBufLen = needed;
  }

  Module._cw_get_biome_bulk(cwGen, biomeBuf, tileX, tileZ, px, px, scale);

  /* Lis le buffer WASM */
  const heap32 = new Int32Array(Module.HEAP32.buffer, biomeBuf, needed);

  /* Construit l'ImageData RGBA */
  const rgba = new Uint8ClampedArray(px * px * 4);
  for (let i = 0; i < needed; i++) {
    const bid = heap32[i];
    const [r,g,b] = biomeColor(bid);
    rgba[i*4]   = r;
    rgba[i*4+1] = g;
    rgba[i*4+2] = b;
    rgba[i*4+3] = 255;
  }
  return rgba;
}

/* ────────────────────────────────────────────────────────────
   STRUCTURES
──────────────────────────────────────────────────────────── */
const STRUCT_TYPES = [
  { id:0,  name:'Village',           icon:'🏘',  color:'#F5A623' },
  { id:1,  name:'Forteresse Nether', icon:'🔥',  color:'#FF4444' },
  { id:2,  name:'Bastion',           icon:'🏰',  color:'#CC3333' },
  { id:3,  name:'Monument',          icon:'🔷',  color:'#4A90D9' },
  { id:4,  name:'Manoir',            icon:'🏚',  color:'#7B4F3A' },
  { id:5,  name:'Temple Désert',     icon:'🔺',  color:'#F5A623' },
  { id:6,  name:'Temple Jungle',     icon:'🌿',  color:'#5E9B3B' },
  { id:7,  name:'Cabane Marais',     icon:'🪄',  color:'#6B8E23' },
  { id:11, name:'Avant-Poste',       icon:'🗼',  color:'#999977' },
  { id:12, name:'Cité Antique',      icon:'💀',  color:'#4444AA' },
];
const MAX_STRUCT_PER_TYPE = 256;

function getStructures(seed, version, radiusChunks) {
  if (!Module || !cwGen) return {};

  const vint    = versionToInt(version);
  const [lo,hi] = seedParts(seed);

  /* Alloue 2 buffers int32 pour x et z */
  const bufX = Module._malloc(MAX_STRUCT_PER_TYPE * 4);
  const bufZ = Module._malloc(MAX_STRUCT_PER_TYPE * 4);

  const result = {};

  for (const st of STRUCT_TYPES) {
    const count = Module._cw_get_structures(
      cwGen, bufX, bufZ,
      st.id,
      0, 0,                /* chunk central : spawn (0,0) */
      radiusChunks,
      MAX_STRUCT_PER_TYPE,
      lo, hi
    );

    const positions = [];
    const hx = new Int32Array(Module.HEAP32.buffer, bufX, count);
    const hz = new Int32Array(Module.HEAP32.buffer, bufZ, count);
    for (let i = 0; i < count; i++) {
      positions.push({ x: hx[i], z: hz[i] });
    }

    result[st.id] = {
      cfg: st,
      positions: positions
    };
  }

  Module._free(bufX);
  Module._free(bufZ);
  return result;
}

/* ────────────────────────────────────────────────────────────
   GESTIONNAIRE DE MESSAGES
──────────────────────────────────────────────────────────── */
self.onmessage = function(e) {
  const msg = e.data;

  switch (msg.type) {

    /* ── Initialiser la seed/version ── */
    case 'init': {
      if (!isReady) {
        postMessage({ type: 'error', msg: 'WASM pas encore chargé.' });
        return;
      }
      const ok = initGen(msg.seed, msg.version, msg.largeBiomes || false);
      postMessage({ type: 'init_ok', ok: ok });
      break;
    }

    /* ── Générer une tuile ── */
    case 'tile': {
      if (!cwGen) {
        postMessage({ type: 'tile', id: msg.id, rgba: null });
        return;
      }
      const rgba = generateTile(msg.tileX, msg.tileZ, msg.tileSz, msg.px);
      /* Transfère le buffer (zero-copy) */
      postMessage(
        { type: 'tile', id: msg.id, rgba: rgba, px: msg.px },
        rgba ? [rgba.buffer] : []
      );
      break;
    }

    /* ── Récupérer les structures ── */
    case 'structures': {
      if (!cwGen) {
        postMessage({ type: 'structures', data: {} });
        return;
      }
      const data = getStructures(msg.seed, msg.version, msg.radiusChunks || 1600);
      postMessage({ type: 'structures', data: data });
      break;
    }
  }
};
