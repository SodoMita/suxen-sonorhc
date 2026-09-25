#include "ag_distortion.h"
#include <math.h>
#include <string.h>

void ag_dist_init(AgDistortion *d, float sr) {
    memset(d,0,sizeof(*d));
    d->sr = sr>0?sr:44100;
    d->drive=2.0f; d->mix=0.5f; d->tone=4000.0f;
}
void ag_dist_set(AgDistortion *d, float drive, float mix, float tone) {
    d->drive=ag_clamp_f(drive,1.0f,20.0f);
    d->mix=ag_clamp_f(mix,0,1);
    d->tone=ag_clamp_f(tone,100,20000);
}
float ag_dist_process(AgDistortion *d, float in) {
    float driven = in * d->drive;
    float distorted = tanhf(driven);
    /* tone control - simple LP */
    static float lp=0; /* not ideal but simple - we should have state, but for simplicity */
    /* Use one-pole approx */
    float cutoff = d->tone / d->sr;
    if(cutoff>0.5f) cutoff=0.5f;
    lp = lp + cutoff * (distorted - lp);
    float out = in*(1.0f-d->mix) + lp*d->mix;
    return out;
}

void ag_bitcrush_init(AgBitcrush *bc, float sr) {
    memset(bc,0,sizeof(*bc));
    bc->sr=sr>0?sr:44100;
    bc->bit_depth=8; bc->sample_rate_div=4;
    bc->hold_time = bc->sample_rate_div / bc->sr;
}
void ag_bitcrush_set(AgBitcrush *bc, float bits, float sr_div) {
    bc->bit_depth=ag_clamp_f(bits,1,16);
    bc->sample_rate_div=ag_clamp_f(sr_div,1,100);
    bc->hold_time = bc->sample_rate_div / bc->sr;
}
float ag_bitcrush_process(AgBitcrush *bc, float in) {
    bc->timer += 1.0/bc->sr;
    if(bc->timer >= bc->hold_time){
        bc->timer=0;
        bc->last=in;
        float steps = powf(2.0f, bc->bit_depth);
        bc->hold = floorf(bc->last * steps) / steps;
    }
    return bc->hold;
}

void ag_wavefolder_init(AgWavefolder *wf) {
    wf->threshold=1.0f; wf->gain=1.0f;
}
float ag_wavefolder_process(AgWavefolder *wf, float in) {
    float x = in * wf->gain;
    float th = wf->threshold;
    if(x>th) x = th - fmodf(x - th, th*2);
    if(x<-th) x = -th + fmodf(-x - th, th*2);
    return x;
}

void ag_comp_init(AgCompressor *c, float sr) {
    memset(c,0,sizeof(*c));
    c->sr=sr>0?sr:44100;
    c->threshold_db=-20; c->ratio=4; c->attack_ms=10; c->release_ms=100; c->makeup_db=0;
}
void ag_comp_set(AgCompressor *c, float thresh_db, float ratio, float attack_ms, float release_ms, float makeup_db) {
    c->threshold_db=thresh_db; c->ratio=ratio>1?ratio:1; c->attack_ms=attack_ms; c->release_ms=release_ms; c->makeup_db=makeup_db;
}
float ag_comp_process(AgCompressor *c, float in) {
    float env_rate_attack = expf(-1.0f / (c->attack_ms * 0.001f * c->sr));
    float env_rate_release = expf(-1.0f / (c->release_ms * 0.001f * c->sr));
    float abs_in = fabsf(in);
    if(abs_in > c->env) c->env = env_rate_attack * c->env + (1-env_rate_attack)*abs_in;
    else c->env = env_rate_release * c->env + (1-env_rate_release)*abs_in;
    float env_db = ag_lin_to_db(c->env);
    float gain_db = 0;
    if(env_db > c->threshold_db){
        float excess = env_db - c->threshold_db;
        gain_db = excess * (1.0f/c->ratio - 1.0f);
    }
    gain_db += c->makeup_db;
    float gain_lin = ag_db_to_lin(gain_db);
    return in * gain_lin;
}
