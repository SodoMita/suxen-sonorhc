#ifndef AG_NATURE_H
#define AG_NATURE_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_filter.h"
#include "ag_noise.h"
#include "ag_envelope.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Nature sounds - birds, insects, frogs, etc. procedural */

/* Bird call - FM chirp with envelope */
typedef struct AgBird {
    AgOsc osc;
    AgOsc mod;
    AgEnv env;
    AgBiquad filter;
    AgRng rng;
    double sr;
    double timer;
    double next_call;
    float gain;
    int active;
    int species; /* 0=sparrow,1=robin,2=crow,3=owl,4=seagull */
    float base_freq;
} AgBird;

void ag_bird_init(AgBird *b, double sr, int species);
void ag_bird_trigger(AgBird *b);
float ag_bird_next(AgBird *b);
void ag_bird_auto(AgBird *b, float density); /* density 0..1 controls call frequency */

/* Cricket - 2 sines AM */
typedef struct AgCricket {
    AgOsc osc1, osc2;
    AgOsc am_lfo;
    AgEnv env;
    AgRng rng;
    double sr;
    double timer;
    double next_chirp;
    float gain;
    int active;
} AgCricket;

void ag_cricket_init(AgCricket *c, double sr);
float ag_cricket_next(AgCricket *c);
void ag_cricket_auto(AgCricket *c, float density);

/* Cicada - bandpassed noise with AM */
typedef struct AgCicada {
    AgNoise noise;
    AgBiquad bp;
    AgOsc am_lfo;
    AgRng rng;
    double sr;
    double timer;
    float gain;
} AgCicada;

void ag_cicada_init(AgCicada *c, double sr);
float ag_cicada_next(AgCicada *c);

/* Frog - low croak with formant */
typedef struct AgFrog {
    AgOsc osc;
    AgBiquad formant;
    AgEnv env;
    AgRng rng;
    double sr;
    double timer;
    double next_croak;
    float gain;
    int active;
} AgFrog;

void ag_frog_init(AgFrog *f, double sr);
float ag_frog_next(AgFrog *f);
void ag_frog_auto(AgFrog *f, float density);

/* Insect swarm - many tiny grains */
#define AG_SWARM_MAX 16
typedef struct AgInsectGrain {
    AgOsc osc;
    AgEnv env;
    float pan;
    int active;
} AgInsectGrain;

typedef struct AgInsectSwarm {
    AgInsectGrain grains[AG_SWARM_MAX];
    AgRng rng;
    double sr;
    double timer;
    float gain;
    float density;
} AgInsectSwarm;

void ag_swarm_init(AgInsectSwarm *s, double sr);
float ag_swarm_next(AgInsectSwarm *s);
void ag_swarm_next_stereo(AgInsectSwarm *s, float *l, float *r);

/* Owl hoot */
typedef struct AgOwl {
    AgOsc osc;
    AgEnv env;
    AgBiquad filter;
    AgRng rng;
    double sr;
    double timer;
    double next_hoot;
    float gain;
    int active;
} AgOwl;

void ag_owl_init(AgOwl *o, double sr);
float ag_owl_next(AgOwl *o);
void ag_owl_auto(AgOwl *o, float density);

#ifdef __cplusplus
}
#endif

#endif
