/**
 * cubiomes_wrapper.c
 * Wrapper léger exposant cubiomes en WASM via Emscripten.
 * Compatible cubiomes master (2024+)
 *
 * Corrections vs version précédente :
 *   - enum StructureType au lieu de StructureType (C strict)
 *   - Bastion_Remnant au lieu de BastionRemnant
 *   - End_Stronghold au lieu de Stronghold
 *   - Mineshaft supprimé (pas de getStructurePos pour lui)
 */

#include <stdlib.h>
#include <string.h>
#include "cubiomes/generator.h"
#include "cubiomes/finders.h"
#include "cubiomes/util.h"

/* ─── Mapping version int → constante cubiomes ─── */
static int version_const(int v) {
    /* v = 1160, 1170, 1180, 1190, 1200, 1210, 1214 */
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

void cw_set_seed(CWGen *cw, int seed_lo, int seed_hi) {
    if (!cw) return;
    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    applySeed(&cw->g, DIM_OVERWORLD, seed);
}

int cw_get_biome(CWGen *cw, int x, int z, int scale) {
    if (!cw) return 0;
    return getBiomeAt(&cw->g, scale, x, 0, z);
}

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

/*
 * Mapping JS id → enum StructureType cubiomes (master 2024+)
 * Noms vérifiés depuis les erreurs de compilation :
 *   - BastionRemnant  → Bastion_Remnant
 *   - Stronghold      → End_Stronghold  (ou absent selon version)
 *   - Mineshaft       → retiré (pas supporté par getStructurePos)
 *
 * Utilise `int` comme retour pour éviter le problème "must use enum tag"
 * (compatibilité C89/C99 strict avec clang)
 */
static int struct_type_id_to_enum(int t) {
    switch(t) {
        case 0:  return Village;
        case 1:  return Fortress;
        case 2:  return Bastion_Remnant;
        case 3:  return Monument;
        case 4:  return Mansion;
        case 5:  return Desert_Pyramid;
        case 6:  return Jungle_Pyramid;
        case 7:  return Swamp_Hut;
        case 8:  return Outpost;
        case 9:  return Ruined_Portal;
        case 10: return Ancient_City;
        default: return Village;
    }
}

int cw_get_structures(CWGen *cw, int *out_x, int *out_z,
                      int struct_type_id, int cx, int cz,
                      int radius, int max, int seed_lo, int seed_hi) {
    if (!cw || !out_x || !out_z) return 0;

    uint64_t seed = ((uint64_t)(unsigned int)seed_hi << 32) | (unsigned int)seed_lo;
    int st = struct_type_id_to_enum(struct_type_id);

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
