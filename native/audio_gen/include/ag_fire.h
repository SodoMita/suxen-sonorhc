#ifndef AG_FIRE_H
#define AG_FIRE_H

#include "ag_common.h"
#include "ag_noise.h"
#include "ag_filter.h"
#include "ag_osc.h"
#include "ag_envelope.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AgFire {
    AgNoise noise;
    AgBiquad lp;
    AgBiquad bp;
    AgBiquad hp;
    AgRng rng;
    AgOsc flicker_lfo;
    double sr;
    float gain;
    float crackle_density;
    float base_intensity;
    double timer;
} AgFire;

void ag_fire_init(AgFire *f, double sr);
void ag_fire_set(AgFire *f, float intensity, float crackle_density);
float ag_fire_next(AgFire *f);

typedef struct AgFireplace {
    AgFire fire;
    AgBiquad room_lp;
    AgOsc room_lfo;
    float room_gain;
} AgFireplace;

void ag_fireplace_init(AgFireplace *fp, double sr);
float ag_fireplace_next(AgFireplace *fp);

typedef struct AgTorch {
    AgFire fire;
    AgOsc wind_lfo;
} AgTorch;

void ag_torch_init(AgTorch *t, double sr);
float ag_torch_next(AgTorch *t);

typedef struct AgBonfire {
    AgFire fire;
    AgNoise low_rumble;
    AgBiquad rumble_lp;
} AgBonfire;

void ag_bonfire_init(AgBonfire *bf, double sr);
float ag_bonfire_next(AgBonfire *bf);

#ifdef __cplusplus
}
#endif

#endif
