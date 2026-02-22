/**
 * cubiomes_wrapper.c
 * Wrapper léger exposant cubiomes en WASM via Emscripten.
 *
 * Compilation (dans le dossier build/) :
 *   emcc cubiomes_wrapper.c cubiomes/generator.c cubiomes/biomes.c \
 *        cubiomes/layers.c cubiomes/noise.c cubiomes/util.c \
 *        cubiomes/finders.c cubiomes/quadbase.c \
 *        -o ../assets/js/cubiomes.js \
 *        -s WASM=1 \
 *        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
 *        -s EXPORTED_FUNCTIONS='["_malloc","_free","_cw_init","_cw_free","_cw_set_seed","_cw_get_biome","_cw_get_biome_bulk","_cw_get_structures"]' \
 *        -s ALLOW_MEMORY_GROWTH=1 \
 *        -s INITIAL_MEMORY=67108864 \
 *        -O2 \
 *        -s MODULARIZE=1 \
 *        -s EXPORT_NAME="CubiomesModule"
 *
 * Nécessite :
 *   - Emscripten (emsdk) installé et activé
 *   - cubiomes cloné dans build/cubiomes/
 *     git clone https://github.com/Cubitect/cubiomes build/cubiomes
 */

#include <stdlib.h>
#include <string.h>
#include "cubiomes/generator.h"
#include "cubiomes/finders.h"
#include "cubiomes/util.h"

/* ─── Mapping version string → constante cubiomes ─── */
static int version_const(int v) {
    /* v = 1160, 1170, 1180, 1190, 1200, 1210, 1211, 1212, 1214 */
    if (v >= 1214) return MC_1_21;
    if (v >= 1210) return MC_1_21;
    if (v >= 1200) return MC_1_20;
    if (v >= 1190) return MC_1_19;
    if (v >= 1180) return MC_1_18;
    if (v >= 1170) return MC_1_17;
    return MC_1_16;
}

/* ─── Opaque handle ─── */
typedef struct {
    Generator g;
    int       mc_version;
} CWGen;

/* Alloue et initialise un générateur.
   version_int : 1160 | 1170 | 1180 | 1190 | 1200 | 1210 | 1214
   large_biomes : 0 ou 1 */
CWGen* cw_init(int version_int, int large_biomes) {
    CWGen *cw = (CWGen*)malloc(sizeof(CWGen));
    if (!cw) return NULL;
    cw->mc_version = version_const(version_int);
    setupGenerator(&cw->g, cw->mc_version, large_biomes ? LARGE_BIOMES : 0);
    return cw;
}

void cw_free(CWGen *cw) {
    if (cw) free(cw);
}

/* Applique la seed.
   seed_lo, seed_hi : les 32 bits bas et hauts d'un int64.
   JavaScript : seed_lo = seed | 0,  seed_hi = Math.floor(seed / 2**32) */
void cw_set_seed(CWGen *cw, int seed_lo, int seed_hi) {
    if (!cw) return;
    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    applySeed(&cw->g, DIM_OVERWORLD, seed);
}

/* Retourne l'id biome au point (x, z).
   scale : 1 = résolution bloc, 4 = résolution quart-chunk (plus rapide) */
int cw_get_biome(CWGen *cw, int x, int z, int scale) {
    if (!cw) return 0;
    return getBiomeAt(&cw->g, scale, x, 0, z);
}

/*
 * Calcule un bloc de biomes en une seule passe.
 * out     : pointeur vers buffer int alloué par JS (malloc)
 * ox, oz  : origine en blocs
 * w, h    : largeur/hauteur en "pixels biome" (à scale donnée)
 * scale   : résolution (1, 4, 16, 64...)
 *
 * Rempli out[z*w + x] = biome_id
 */
void cw_get_biome_bulk(CWGen *cw, int *out, int ox, int oz, int w, int h, int scale) {
    if (!cw || !out) return;
    /* cubiomes : genBiomes() ou getOverworldBiomes() selon version */
    Range r;
    r.scale = scale;
    r.x = ox / scale;
    r.z = oz / scale;
    r.sx = w;
    r.sz = h;
    r.y  = 0;  /* surface */
    r.sy = 1;
    genBiomes(&cw->g, out, r);
}

/* ─── STRUCTURES ─── */

/* Type de structure → numéro cubiomes */
static StructureType struct_type(int t) {
    /* 0=village 1=fortress 2=bastionremnant 3=monument 4=mansion
       5=desert_pyramid 6=jungle_pyramid 7=swamp_hut 8=stronghold
       9=mineshaft 10=ruined_portal 11=pillager_outpost 12=ancient_city */
    switch(t) {
        case 0:  return Village;
        case 1:  return Fortress;
        case 2:  return BastionRemnant;
        case 3:  return Monument;
        case 4:  return Mansion;
        case 5:  return Desert_Pyramid;
        case 6:  return Jungle_Pyramid;
        case 7:  return Swamp_Hut;
        case 8:  return Stronghold;
        case 9:  return Mineshaft;
        case 10: return Ruined_Portal;
        case 11: return Outpost;
        case 12: return Ancient_City;
        default: return Village;
    }
}

/*
 * Cherche les positions de structures dans un rayon.
 * out_x, out_z : buffers int[] alloués par JS (malloc, taille = max*4 bytes)
 * struct_type_id : voir ci-dessus
 * cx, cz : chunk central
 * radius  : rayon en chunks
 * max     : nombre max de résultats
 * Retourne : nombre de structures trouvées
 */
int cw_get_structures(CWGen *cw, int *out_x, int *out_z,
                      int struct_type_id, int cx, int cz,
                      int radius, int max, int seed_lo, int seed_hi) {
    if (!cw || !out_x || !out_z) return 0;

    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    StructureType st = struct_type(struct_type_id);

    /* Config de la structure */
    StructureConfig sc;
    if (getStructureConfig(st, cw->mc_version, &sc) != 1) return 0;

    int count = 0;
    int step = sc.regionSize;
    if (step < 1) step = 1;

    int rmin = cx / step - radius / step - 2;
    int rmax = cx / step + radius / step + 2;

    for (int rx = rmin; rx <= rmax && count < max; rx++) {
        for (int rz = rmin; rz <= rmax && count < max; rz++) {
            Pos p;
            if (!getStructurePos(st, cw->mc_version, seed, rx, rz, &p)) continue;

            /* Filtre par rayon en chunks */
            int dcx = p.x / 16 - cx;
            int dcz = p.z / 16 - cz;
            if (dcx*dcx + dcz*dcz > (long long)radius*radius) continue;

            /* Vérification biome (simplifié) */
            out_x[count] = p.x;
            out_z[count] = p.z;
            count++;
        }
    }
    return count;
}
