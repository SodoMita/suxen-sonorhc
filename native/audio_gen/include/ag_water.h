#ifndef AG_WATER_H
#define AG_WATER_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_filter.h"
#include "ag_noise.h"
#include "ag_envelope.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Water sounds - ocean, river, waterfall, drips, bubbles, underwater */

typedef struct AgOcean {
    AgNoise noise;
    AgBiquad lp;
    AgBiquad bp;
    AgOsc swell_lfo; /* low freq for wave swell */
    AgOsc crash_lfo;
    AgRng rng;
    double sr;
    float gain;
    float swell_strength;
    float crash_chance;
    double crash_timer;
    float base_freq;
} AgOcean;

void ag_ocean_init(AgOcean *o, double sr);
void ag_ocean_set(AgOcean *o, float gain, float swell_strength);
float ag_ocean_next(AgOcean *o);

typedef struct AgRiver {
    AgNoise noise;
    AgBiquad lp;
    AgBiquad hp;
    AgOsc flow_lfo;
    AgRng rng;
    double sr;
    float gain;
    float turbulence;
} AgRiver;

void ag_river_init(AgRiver *r, double sr);
float ag_river_next(AgRiver *r);

typedef struct AgStream {
    AgRiver river;
    AgBiquad sparkle_bp;
    AgRng rng;
    float sparkle_chance;
} AgStream;

void ag_stream_init(AgStream *s, double sr);
float ag_stream_next(AgStream *s);

typedef struct AgWaterfall {
    AgNoise noise;
    AgBiquad lp1, lp2;
    AgBiquad hp;
    AgOsc roar_lfo;
    double sr;
    float gain;
    float roar;
} AgWaterfall;

void ag_waterfall_init(AgWaterfall *wf, double sr);
float ag_waterfall_next(AgWaterfall *wf);

typedef struct AgDrip {
    AgOsc osc;
    AgEnv env;
    AgBiquad filter;
    AgRng rng;
    double sr;
    double timer;
    double next_drip_time;
    float gain;
    int active;
    float drip_freq;
} AgDrip;

void ag_drip_init(AgDrip *d, double sr);
void ag_drip_trigger(AgDrip *d, float freq, float gain);
float ag_drip_next(AgDrip *d);
void ag_drip_auto(AgDrip *d, float density); /* density 0..1 controls random drips */

typedef struct AgBubbles {
    AgRng rng;
    AgOsc osc;
    AgEnv env;
    double sr;
    double timer;
    double next_bubble;
    float gain;
    float rate;
    int active;
} AgBubbles;

void ag_bubbles_init(AgBubbles *b, double sr);
float ag_bubbles_next(AgBubbles *b);
void ag_bubbles_set_rate(AgBubbles *b, float rate);

typedef struct AgUnderwaterAmbience {
    AgRiver base;
    AgBiquad muffle_lp;
    AgOsc pressure_lfo;
    double sr;
    float gain;
} AgUnderwaterAmbience;

void ag_underwater_amb_init(AgUnderwaterAmbience *uw, double sr);
float ag_underwater_amb_next(AgUnderwaterAmbience *uw);

#ifdef __cplusplus
}
#endif

#endif
