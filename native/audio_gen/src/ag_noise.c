#include "ag_noise.h"
#include <string.h>

void ag_noise_init(AgNoise *n, uint64_t seed) {
    memset(n,0,sizeof(*n));
    ag_rng_seed(&n->rng, seed);
    n->pink_scale = 0.11f;
}
float ag_noise_white(AgNoise *n) {
    return ag_rng_range_f32(&n->rng, -1.0f, 1.0f);
}
float ag_noise_pink(AgNoise *n) {
    float white = ag_noise_white(n);
    n->b0 = 0.99886f * n->b0 + white * 0.0555179f;
    n->b1 = 0.99332f * n->b1 + white * 0.0750759f;
    n->b2 = 0.96900f * n->b2 + white * 0.1538520f;
    n->b3 = 0.86650f * n->b3 + white * 0.3104856f;
    n->b4 = 0.55000f * n->b4 + white * 0.5329522f;
    n->b5 = -0.7616f * n->b5 - white * 0.0168980f;
    float out = (n->b0 + n->b1 + n->b2 + n->b3 + n->b4 + n->b5 + n->b6 + white * 0.5362f) * n->pink_scale;
    n->b6 = white * 0.115926f;
    return ag_clamp_f(out, -1.0f, 1.0f);
}
float ag_noise_brown(AgNoise *n) {
    float white = ag_noise_white(n);
    n->brown += white * 0.02f;
    /* leaky integrator */
    n->brown *= 0.998f;
    return ag_clamp_f(n->brown * 3.5f, -1.0f, 1.0f);
}
float ag_noise_velvet(AgNoise *n, int density) {
    /* Velvet noise: sparse impulses - density = average distance in samples, e.g. 32 */
    if (density < 1) density = 32;
    int r = (int)(ag_rng_next_f32(&n->rng) * density);
    if (r==0) {
        return ag_rng_next_f32(&n->rng) > 0.5f ? 1.0f : -1.0f;
    }
    return 0.0f;
}
float ag_noise_crackle(AgNoise *n, float density) {
    /* density 0..1 = probability of crackle */
    float p = ag_rng_next_f32(&n->rng);
    if (p < density) {
        float white = ag_noise_white(n);
        return white * white * white * 8.0f;
    }
    return 0.0f;
}
