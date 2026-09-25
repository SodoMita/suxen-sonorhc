#ifndef AG_FILTER_H
#define AG_FILTER_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AG_FILTER_LP = 0,
    AG_FILTER_HP,
    AG_FILTER_BP,
    AG_FILTER_NOTCH,
    AG_FILTER_PEAK,
    AG_FILTER_LOSHELF,
    AG_FILTER_HISHELF,
    AG_FILTER_COUNT
} AgFilterType;

typedef struct AgBiquad {
    float b0,b1,b2,a1,a2;
    float z1,z2;
} AgBiquad;

void ag_biquad_init(AgBiquad *f);
void ag_biquad_set(AgBiquad *f, AgFilterType type, float freq, float q, float gain_db, float sr);
float ag_biquad_process(AgBiquad *f, float in);
void ag_biquad_process_block(AgBiquad *f, float *buf, int n);

/* One-pole filters - super cheap */
typedef struct AgOnePole {
    float a0,b1;
    float z1;
} AgOnePole;

void ag_onepole_lp(AgOnePole *f, float freq, float sr);
void ag_onepole_hp(AgOnePole *f, float freq, float sr);
float ag_onepole_process(AgOnePole *f, float in);

/* State Variable Filter - LP/HP/BP simultaneous */
typedef struct AgSVF {
    float freq;
    float q;
    float sr;
    float ic1eq, ic2eq;
} AgSVF;

void ag_svf_init(AgSVF *svf, float freq, float q, float sr);
void ag_svf_set(AgSVF *svf, float freq, float q);
void ag_svf_process(AgSVF *svf, float in, float *lp, float *hp, float *bp, float *notch);

/* Simple DC blocker */
typedef struct AgDCBlock {
    float x1,y1;
} AgDCBlock;

void ag_dcblock_init(AgDCBlock *f);
float ag_dcblock_process(AgDCBlock *f, float in);

/* Moog-style 4-pole ladder approx (cheap) */
typedef struct AgLadder {
    float freq;
    float res;
    float sr;
    float stage[4];
    float delay[4];
} AgLadder;

void ag_ladder_init(AgLadder *f, float freq, float res, float sr);
void ag_ladder_set(AgLadder *f, float freq, float res);
float ag_ladder_process(AgLadder *f, float in);

#ifdef __cplusplus
}
#endif

#endif
