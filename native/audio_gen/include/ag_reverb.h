#ifndef AG_REVERB_H
#define AG_REVERB_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Freeverb-style reverb */

#define AG_REVERB_COMBS 8
#define AG_REVERB_ALLP 4

typedef struct AgComb {
    float *buf;
    int size;
    int pos;
    float feedback;
    float damp;
    float filter_store;
} AgComb;

typedef struct AgAllpass {
    float *buf;
    int size;
    int pos;
    float feedback;
} AgAllpass;

typedef struct AgReverb {
    AgComb combs[AG_REVERB_COMBS];
    AgAllpass allps[AG_REVERB_ALLP];
    float *comb_bufs[AG_REVERB_COMBS];
    float *allp_bufs[AG_REVERB_ALLP];
    int sr;
    float room_size;
    float damping;
    float wet;
    float dry;
    float width;
    float gain;
    int initialized;
} AgReverb;

void ag_reverb_init(AgReverb *rv, int sr);
void ag_reverb_free(AgReverb *rv);
void ag_reverb_set_room_size(AgReverb *rv, float room_size); /* 0..1 */
void ag_reverb_set_damping(AgReverb *rv, float damping); /* 0..1 */
void ag_reverb_set_wet(AgReverb *rv, float wet);
void ag_reverb_set_dry(AgReverb *rv, float dry);
void ag_reverb_set_width(AgReverb *rv, float width);
void ag_reverb_set_gain(AgReverb *rv, float gain);
float ag_reverb_process(AgReverb *rv, float in);
void ag_reverb_process_stereo(AgReverb *rv, float in_l, float in_r, float *out_l, float *out_r);
void ag_reverb_process_block(AgReverb *rv, float *buf, int frames);
void ag_reverb_process_block_stereo(AgReverb *rv, float *interleaved, int frames);

/* Simple Schroeder reverb (cheaper) */
typedef struct AgSchroeder {
    float *comb_bufs[4];
    int comb_sizes[4];
    int comb_pos[4];
    float comb_fb[4];
    float *allp_bufs[2];
    int allp_sizes[2];
    int allp_pos[2];
    int sr;
    float wet, dry;
    int initialized;
} AgSchroeder;

void ag_schroeder_init(AgSchroeder *rv, int sr);
void ag_schroeder_free(AgSchroeder *rv);
float ag_schroeder_process(AgSchroeder *rv, float in);

#ifdef __cplusplus
}
#endif

#endif
