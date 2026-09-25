#ifndef AG_AMBIENT_H
#define AG_AMBIENT_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_filter.h"
#include "ag_noise.h"
#include "ag_envelope.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ambient textures - wind, rain, drone, pad, etc. */

typedef struct AgDrone {
    AgOsc osc1, osc2, osc3;
    AgBiquad filter;
    AgLFO lfo;
    AgNoise noise;
    float gain;
    double sr;
    float detune;
} AgDrone;

void ag_drone_init(AgDrone *d, double sr, float base_freq);
void ag_drone_set_freq(AgDrone *d, float freq);
float ag_drone_next(AgDrone *d);

/* Wind */
typedef struct AgWindGen {
    AgNoise noise;
    AgBiquad lp1, lp2;
    AgOnePole smooth;
    AgLFO gust_lfo;
    float gain;
    double sr;
    float gust_strength;
} AgWindGen;

void ag_wind_init(AgWindGen *w, double sr);
void ag_wind_set_strength(AgWindGen *w, float strength);
float ag_wind_next(AgWindGen *w);

/* Rain */
typedef struct AgRain {
    AgNoise noise;
    AgBiquad bp;
    AgRng rng;
    double sr;
    float density;
    float gain;
    double timer;
} AgRain;

void ag_rain_init(AgRain *r, double sr);
void ag_rain_set_density(AgRain *r, float density);
float ag_rain_next(AgRain *r);

/* Granular pad - cloud of grains */
#define AG_GRAIN_MAX 32
#define AG_GRAIN_LEN 1024

typedef struct AgGrain {
    float buf[AG_GRAIN_LEN];
    int pos;
    int len;
    float pan_l, pan_r;
    float gain;
    int active;
    double env_phase;
} AgGrain;

typedef struct AgGranularPad {
    AgGrain grains[AG_GRAIN_MAX];
    AgRng rng;
    double sr;
    float base_freq;
    float spread;
    float grain_rate;
    double timer;
    float gain;
    AgBiquad filter;
} AgGranularPad;

void ag_granular_init(AgGranularPad *gp, double sr, float base_freq);
void ag_granular_set(AgGranularPad *gp, float freq, float spread, float rate);
float ag_granular_next(AgGranularPad *gp);
void ag_granular_next_stereo(AgGranularPad *gp, float *l, float *r);

/* Shimmer pad - octave-up reverb tail simulation */
typedef struct AgShimmer {
    AgOsc osc;
    AgBiquad filter;
    float feedback;
    float buf[44100]; /* 1 sec delay */
    int buf_pos;
    double sr;
    float gain;
} AgShimmer;

void ag_shimmer_init(AgShimmer *sh, double sr, float base_freq);
float ag_shimmer_next(AgShimmer *sh, float in);

/* Underwater / muffled texture */
typedef struct AgUnderwater {
    AgBiquad lp;
    AgBiquad hp;
    AgOsc lfo;
    float gain;
    double sr;
} AgUnderwater;

void ag_underwater_init(AgUnderwater *uw, double sr);
float ag_underwater_process(AgUnderwater *uw, float in);

#ifdef __cplusplus
}
#endif

#endif
