#ifndef AG_DISTORTION_H
#define AG_DISTORTION_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Distortion / saturation / bitcrusher / etc */

typedef struct AgDistortion {
    float drive;      /* 1..20 */
    float mix;        /* 0..1 dry/wet */
    float tone;       /* LP cutoff for harshness */
    float sr;
} AgDistortion;

void ag_dist_init(AgDistortion *d, float sr);
void ag_dist_set(AgDistortion *d, float drive, float mix, float tone);
float ag_dist_process(AgDistortion *d, float in);

/* Bitcrusher */
typedef struct AgBitcrush {
    float bit_depth;  /* 1..16 */
    float sample_rate_div; /* 1..100 */
    float sr;
    float hold;
    float last;
    double timer;
    double hold_time;
} AgBitcrush;

void ag_bitcrush_init(AgBitcrush *bc, float sr);
void ag_bitcrush_set(AgBitcrush *bc, float bits, float sr_div);
float ag_bitcrush_process(AgBitcrush *bc, float in);

/* Wavefolder */
typedef struct AgWavefolder {
    float threshold;
    float gain;
} AgWavefolder;

void ag_wavefolder_init(AgWavefolder *wf);
float ag_wavefolder_process(AgWavefolder *wf, float in);

/* Simple compressor */
typedef struct AgCompressor {
    float threshold_db;
    float ratio;
    float attack_ms;
    float release_ms;
    float makeup_db;
    float sr;
    float env;
} AgCompressor;

void ag_comp_init(AgCompressor *c, float sr);
void ag_comp_set(AgCompressor *c, float thresh_db, float ratio, float attack_ms, float release_ms, float makeup_db);
float ag_comp_process(AgCompressor *c, float in);

#ifdef __cplusplus
}
#endif

#endif
