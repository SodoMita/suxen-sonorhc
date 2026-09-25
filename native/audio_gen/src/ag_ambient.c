#include "ag_ambient.h"
#include "ag_filter.h"
#include <math.h>
#include <string.h>

void ag_drone_init(AgDrone *d, double sr, float base_freq) {
    memset(d,0,sizeof(*d));
    d->sr = sr>0?sr:AG_SR_DEFAULT;
    d->gain = 0.5f;
    d->detune = 0.02f;
    ag_osc_init(&d->osc1, AG_OSC_SINE, sr);
    ag_osc_init(&d->osc2, AG_OSC_SINE, sr);
    ag_osc_init(&d->osc3, AG_OSC_SINE, sr);
    ag_osc_set_freq(&d->osc1, base_freq);
    ag_osc_set_freq(&d->osc2, base_freq*1.01);
    ag_osc_set_freq(&d->osc3, base_freq*0.995);
    ag_biquad_init(&d->filter);
    ag_biquad_set(&d->filter, AG_FILTER_LP, base_freq*2.0f, 0.7f, 0, (float)sr);
    ag_lfo_init(&d->lfo, AG_OSC_SINE, 0.1, sr, 0.3f);
    ag_noise_init(&d->noise, 0xD10);
}
void ag_drone_set_freq(AgDrone *d, float freq) {
    ag_osc_set_freq(&d->osc1, freq);
    ag_osc_set_freq(&d->osc2, freq*(1.0+d->detune));
    ag_osc_set_freq(&d->osc3, freq*(1.0-d->detune*0.5));
    ag_biquad_set(&d->filter, AG_FILTER_LP, freq*3.0f, 0.7f, 0, (float)d->sr);
}
float ag_drone_next(AgDrone *d) {
    float s1 = ag_osc_next(&d->osc1);
    float s2 = ag_osc_next(&d->osc2);
    float s3 = ag_osc_next(&d->osc3);
    float lfo = ag_lfo_next(&d->lfo);
    float mix = (s1 + s2*0.7f + s3*0.7f)/2.4f;
    mix += ag_noise_white(&d->noise) * 0.02f;
    mix = ag_biquad_process(&d->filter, mix);
    mix *= (1.0f + lfo*0.2f);
    return mix * d->gain;
}

/* Wind */
void ag_wind_init(AgWindGen *w, double sr) {
    memset(w,0,sizeof(*w));
    w->sr = sr>0?sr:AG_SR_DEFAULT;
    w->gain = 0.5f;
    w->gust_strength = 0.3f;
    ag_noise_init(&w->noise, 0x111);
    ag_biquad_init(&w->lp1);
    ag_biquad_init(&w->lp2);
    ag_biquad_set(&w->lp1, AG_FILTER_LP, 800.0f, 0.7f, 0, (float)sr);
    ag_biquad_set(&w->lp2, AG_FILTER_LP, 400.0f, 0.7f, 0, (float)sr);
    ag_onepole_lp(&w->smooth, 2.0f, (float)sr);
    ag_lfo_init(&w->gust_lfo, AG_OSC_SINE, 0.15, sr, 1.0f);
}
void ag_wind_set_strength(AgWindGen *w, float strength) {
    w->gust_strength = ag_clamp_f(strength, 0, 1);
}
float ag_wind_next(AgWindGen *w) {
    float white = ag_noise_white(&w->noise);
    float pink = ag_noise_pink(&w->noise);
    float n = white*0.3f + pink*0.7f;
    n = ag_biquad_process(&w->lp1, n);
    n = ag_biquad_process(&w->lp2, n);
    float gust = ag_lfo_next(&w->gust_lfo) * w->gust_strength;
    float env = 0.5f + gust*0.5f;
    env = ag_onepole_process(&w->smooth, env);
    return n * env * w->gain;
}

/* Rain */
void ag_rain_init(AgRain *r, double sr) {
    memset(r,0,sizeof(*r));
    r->sr = sr>0?sr:AG_SR_DEFAULT;
    r->gain = 0.4f;
    r->density = 0.3f;
    ag_noise_init(&r->noise, 0xA11A);
    ag_rng_seed(&r->rng, 0xA11A);
    ag_biquad_init(&r->bp);
    ag_biquad_set(&r->bp, AG_FILTER_BP, 2500.0f, 1.5f, 0, (float)sr);
}
void ag_rain_set_density(AgRain *r, float density) { r->density = ag_clamp_f(density,0,1); }
float ag_rain_next(AgRain *r) {
    r->timer += 1.0/r->sr;
    float out = 0.0f;
    if (ag_rng_next_f32(&r->rng) < r->density * 0.1f) {
        float drop = ag_rng_range_f32(&r->rng, 0.3f, 1.0f);
        float freq = ag_rng_range_f32(&r->rng, 1500.0f, 6000.0f);
        ag_biquad_set(&r->bp, AG_FILTER_BP, freq, 2.0f, 0, (float)r->sr);
        float n = ag_noise_white(&r->noise) * drop;
        n = ag_biquad_process(&r->bp, n);
        out += n * 0.8f;
    }
    float drizzle = ag_noise_white(&r->noise) * 0.05f;
    drizzle = ag_biquad_process(&r->bp, drizzle);
    out += drizzle;
    return out * r->gain;
}

/* Granular */
void ag_granular_init(AgGranularPad *gp, double sr, float base_freq) {
    memset(gp,0,sizeof(*gp));
    gp->sr = sr>0?sr:AG_SR_DEFAULT;
    gp->base_freq = base_freq>0?base_freq:110.0f;
    gp->spread = 0.1f;
    gp->grain_rate = 20.0f;
    gp->gain = 0.6f;
    ag_rng_seed(&gp->rng, 0x611A);
    ag_biquad_init(&gp->filter);
    ag_biquad_set(&gp->filter, AG_FILTER_LP, base_freq*4.0f, 0.7f, 0, (float)sr);
    for (int i=0;i<AG_GRAIN_MAX;i++) gp->grains[i].active=0;
}
void ag_granular_set(AgGranularPad *gp, float freq, float spread, float rate) {
    gp->base_freq = freq;
    gp->spread = spread;
    gp->grain_rate = rate;
    ag_biquad_set(&gp->filter, AG_FILTER_LP, freq*4.0f, 0.7f, 0, (float)gp->sr);
}
static void spawn_grain(AgGranularPad *gp) {
    for (int i=0;i<AG_GRAIN_MAX;i++) {
        if (!gp->grains[i].active) {
            AgGrain *g = &gp->grains[i];
            float detune = ag_rng_range_f32(&gp->rng, -gp->spread, gp->spread);
            float freq = gp->base_freq * powf(2.0f, detune);
            int len = (int)(gp->sr / freq * 2); /* 2 cycles */
            if (len < 64) len=64;
            if (len > AG_GRAIN_LEN) len=AG_GRAIN_LEN;
            g->len = len;
            g->pos = 0;
            g->env_phase = 0.0;
            for (int j=0;j<len;j++) {
                float ph = (float)j / len;
                float win = sinf(ph * (float)AG_PI); /* hanning */
                float osc = sinf(ph * (float)AG_TAU * 2.0f);
                g->buf[j] = osc * win;
            }
            float pan = ag_rng_range_f32(&gp->rng, -1.0f, 1.0f);
            ag_buffer_pan_stereo(1.0f, pan, &g->pan_l, &g->pan_r);
            g->gain = ag_rng_range_f32(&gp->rng, 0.3f, 0.8f);
            g->active=1;
            return;
        }
    }
}
float ag_granular_next(AgGranularPad *gp) {
    float l,r;
    ag_granular_next_stereo(gp,&l,&r);
    return (l+r)*0.5f;
}
void ag_granular_next_stereo(AgGranularPad *gp, float *out_l, float *out_r) {
    gp->timer += 1.0/gp->sr;
    double interval = 1.0 / (gp->grain_rate>0?gp->grain_rate:20.0);
    if (gp->timer >= interval) {
        gp->timer=0;
        spawn_grain(gp);
    }
    float mix_l=0, mix_r=0;
    for (int i=0;i<AG_GRAIN_MAX;i++) {
        AgGrain *g = &gp->grains[i];
        if (!g->active) continue;
        float s = g->buf[g->pos];
        mix_l += s * g->pan_l * g->gain;
        mix_r += s * g->pan_r * g->gain;
        g->pos++;
        if (g->pos >= g->len) g->active=0;
    }
    mix_l = ag_biquad_process(&gp->filter, mix_l);
    mix_r = mix_l * 0.9f + mix_r*0.1f; /* crude */
    *out_l = mix_l * gp->gain;
    *out_r = mix_r * gp->gain;
}

/* Shimmer */
void ag_shimmer_init(AgShimmer *sh, double sr, float base_freq) {
    memset(sh,0,sizeof(*sh));
    sh->sr = sr>0?sr:AG_SR_DEFAULT;
    sh->gain=0.5f;
    sh->feedback=0.6f;
    ag_osc_init(&sh->osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&sh->osc, base_freq*2.0);
    ag_biquad_init(&sh->filter);
    ag_biquad_set(&sh->filter, AG_FILTER_LP, 3000.0f, 0.7f, 0, (float)sr);
}
float ag_shimmer_next(AgShimmer *sh, float in) {
    float delayed = sh->buf[sh->buf_pos];
    float pitch = ag_osc_next(&sh->osc) * 0.5f + 0.5f; /* modulate */
    float out = in + delayed * sh->feedback;
    out = ag_biquad_process(&sh->filter, out);
    sh->buf[sh->buf_pos] = out;
    sh->buf_pos = (sh->buf_pos+1) % 44100;
    return out * sh->gain;
}

/* Underwater */
void ag_underwater_init(AgUnderwater *uw, double sr) {
    memset(uw,0,sizeof(*uw));
    uw->sr = sr>0?sr:AG_SR_DEFAULT;
    uw->gain=1.0f;
    ag_biquad_init(&uw->lp);
    ag_biquad_init(&uw->hp);
    ag_biquad_set(&uw->lp, AG_FILTER_LP, 800.0f, 0.7f, 0, (float)sr);
    ag_biquad_set(&uw->hp, AG_FILTER_HP, 20.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&uw->lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&uw->lfo, 0.3);
}
float ag_underwater_process(AgUnderwater *uw, float in) {
    float lfo = ag_osc_next(&uw->lfo) * 0.2f;
    ag_biquad_set(&uw->lp, AG_FILTER_LP, 600.0f + lfo*200.0f, 0.7f, 0, (float)uw->sr);
    float out = ag_biquad_process(&uw->lp, in);
    out = ag_biquad_process(&uw->hp, out);
    return out * uw->gain;
}
