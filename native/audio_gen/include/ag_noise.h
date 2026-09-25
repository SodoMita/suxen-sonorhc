#ifndef AG_NOISE_H
#define AG_NOISE_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AgNoise {
    AgRng rng;
    /* Pink noise (Paul Kellet) */
    float b0,b1,b2,b3,b4,b5,b6;
    float brown;
    float pink_scale;
} AgNoise;

void ag_noise_init(AgNoise *n, uint64_t seed);
float ag_noise_white(AgNoise *n);
float ag_noise_pink(AgNoise *n);
float ag_noise_brown(AgNoise *n);
float ag_noise_velvet(AgNoise *n, int density); /* -1,0,1 sparse */
float ag_noise_crackle(AgNoise *n, float density); /* occasional impulses */

/* Texture helpers - filter+noise combos are in ag_ambient etc */

#ifdef __cplusplus
}
#endif

#endif
