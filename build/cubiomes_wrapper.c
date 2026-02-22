/**
 * cubiomes_wrapper.c
 * Wrapper léger exposant cubiomes en WASM via Emscripten.
 *
 * Compilation :
 *   emcc cubiomes_wrapper.c cubiomes/*.c \
 *        -o ../assets/js/cubiomes.js \
 *        -s WASM=1 \
 *        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
 *        -s EXPORTED_FUNCTIONS='["_malloc","_free","_cw_init","_cw_free","_cw_set_seed","_cw_get_biome","_cw_get_biome_bulk","_cw_get_structures"]' \
 *        -s ALLOW_MEMORY_GROWTH=1 \
 *        -O2 \
 *        -s MODULARIZE=1 \
 *        -s EXPORT_NAME="CubiomesModule"
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cubiomes/generator.h"
#include "cubiomes/finders.h"
#include "cubiomes/util.h"

/* ─── Mapping version string → constante cubiomes ─── */
static int version_const(int v) {
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

/* Initialise un générateur */
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

/* Applique la seed */
void cw_set_seed(CWGen *cw, int seed_lo, int seed_hi) {
    if (!cw) return;
    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    applySeed(&cw->g, DIM_OVERWORLD, seed);
}

/* Retourne l'id biome au point (x, z) */
int cw_get_biome(CWGen *cw, int x, int z, int scale) {
    if (!cw) return 0;
    return getBiomeAt(&cw->g, scale, x, 0, z);
}

/* Calcule un bloc de biomes */
void cw_get_biome_bulk(CWGen *cw, int *out, int ox, int oz, int w, int h, int scale) {
    if (!cw || !out) return;
    Range r;
    r.scale = scale;
    r.x = ox / scale;
    r.z = oz / scale;
    r.sx = w;
    r.sz = h;
    r.y  = 0;
    r.sy = 1;
    genBiomes(&cw->g, out, r);
}

/* ─── STRUCTURES ─── */

/* Map int → constante cubiomes */
static StructureType struct_type(int t) {
    switch(t) {
        case 0:  return STRUCTURE_VILLAGE;
        case 1:  return STRUCTURE_FORTRESS;
        case 2:  return STRUCTURE_BASTION_REMNANT;
        case 3:  return STRUCTURE_MONUMENT;
        case 4:  return STRUCTURE_MANSION;
        case 5:  return STRUCTURE_DESERT_PYRAMID;
        case 6:  return STRUCTURE_JUNGLE_PYRAMID;
        case 7:  return STRUCTURE_SWAMP_HUT;
        case 8:  return STRUCTURE_STRONGHOLD;
        case 9:  return STRUCTURE_MINESHAFT;
        case 10: return STRUCTURE_RUINED_PORTAL;
        case 11: return STRUCTURE_PILLAGER_OUTPOST;
        case 12: return STRUCTURE_ANCIENT_CITY;
        default: return STRUCTURE_VILLAGE;
    }
}

/* Cherche les positions de structures */
int cw_get_structures(CWGen *cw, int *out_x, int *out_z,
                      int struct_type_id, int cx, int cz,
                      int radius, int max, int seed_lo, int seed_hi) {
    if (!cw || !out_x || !out_z) return 0;

    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    StructureType st = struct_type(struct_type_id);

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

            int dcx = p.x / 16 - cx;
            int dcz = p.z / 16 - cz;
            if (dcx*dcx + dcz*dcz > (long long)radius*radius) continue;

            out_x[count] = p.x;
            out_z[count] = p.z;
            count++;
        }
    }
    return count;
}
