#include "ag_water.h"
#include <string.h>
#include <math.h>

void ag_ocean_init(AgOcean *o, double sr) {
    memset(o,0,sizeof(*o));
    o->sr=sr>0?sr:AG_SR_DEFAULT;
    o->gain=0.5f;
    o->swell_strength=0.4f;
    o->crash_chance=0.02f;
    o->base_freq=80.0f;
    ag_noise_init(&o->noise, 0x0CEA11);
    ag_rng_seed(&o->rng, 0x0CEA11);
    ag_biquad_init(&o->lp);
    ag_biquad_set(&o->lp, AG_FILTER_LP, 800.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&o->bp);
    ag_biquad_set(&o->bp, AG_FILTER_BP, 300.0f, 0.8f, 0, (float)sr);
    ag_osc_init(&o->swell_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&o->swell_lfo, 0.07f);
    ag_osc_init(&o->crash_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&o->crash_lfo, 0.15f);
}
void ag_ocean_set(AgOcean *o, float gain, float swell_strength) {
    o->gain=ag_clamp_f(gain,0,1);
    o->swell_strength=ag_clamp_f(swell_strength,0,1);
}
float ag_ocean_next(AgOcean *o) {
    float white = ag_noise_white(&o->noise);
    float pink = ag_noise_pink(&o->noise);
    float n = white*0.2f + pink*0.8f;
    n = ag_biquad_process(&o->lp, n);
    float swell = ag_osc_next(&o->swell_lfo) * o->swell_strength;
    float crash_mod = ag_osc_next(&o->crash_lfo) * 0.5f + 0.5f;
    n *= (0.6f + swell*0.4f + crash_mod*0.2f);

    /* occasional crash */
    o->crash_timer += 1.0/o->sr;
    if(o->crash_timer > 2.0){
        o->crash_timer=0;
        if(ag_rng_next_f32(&o->rng) < o->crash_chance){
            /* burst of higher freq noise */
            float burst = ag_noise_white(&o->noise) * 2.0f;
            burst = ag_biquad_process(&o->bp, burst);
            n += burst * 0.5f;
        }
    }
    return n * o->gain;
}

void ag_river_init(AgRiver *r, double sr) {
    memset(r,0,sizeof(*r));
    r->sr=sr>0?sr:AG_SR_DEFAULT;
    r->gain=0.4f;
    r->turbulence=0.3f;
    ag_noise_init(&r->noise, 0x1112);
    ag_rng_seed(&r->rng, 0x1112);
    ag_biquad_init(&r->lp);
    ag_biquad_set(&r->lp, AG_FILTER_LP, 1200.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&r->hp);
    ag_biquad_set(&r->hp, AG_FILTER_HP, 40.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&r->flow_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&r->flow_lfo, 0.3f);
}
float ag_river_next(AgRiver *r) {
    float n = ag_noise_pink(&r->noise) * 0.7f + ag_noise_white(&r->noise)*0.3f;
    n = ag_biquad_process(&r->lp, n);
    n = ag_biquad_process(&r->hp, n);
    float flow = ag_osc_next(&r->flow_lfo) * r->turbulence;
    n *= (1.0f + flow*0.3f);
    /* occasional turbulence burst */
    if(ag_rng_next_f32(&r->rng) < 0.001f) n += ag_noise_white(&r->noise)*0.5f;
    return n * r->gain;
}

void ag_stream_init(AgStream *s, double sr) {
    memset(s,0,sizeof(*s));
    ag_river_init(&s->river, sr);
    s->river.gain=0.35f;
    ag_biquad_init(&s->sparkle_bp);
    ag_biquad_set(&s->sparkle_bp, AG_FILTER_BP, 3500.0f, 1.2f, 0, (float)sr);
    ag_rng_seed(&s->rng, 0x51A);
    s->sparkle_chance=0.02f;
}
float ag_stream_next(AgStream *s) {
    float base = ag_river_next(&s->river);
    if(ag_rng_next_f32(&s->rng) < s->sparkle_chance){
        float sparkle = ag_noise_white(&s->river.noise) * 0.8f;
        sparkle = ag_biquad_process(&s->sparkle_bp, sparkle);
        base += sparkle * 0.4f;
    }
    return base;
}

void ag_waterfall_init(AgWaterfall *wf, double sr) {
    memset(wf,0,sizeof(*wf));
    wf->sr=sr>0?sr:AG_SR_DEFAULT;
    wf->gain=0.6f;
    wf->roar=0.5f;
    ag_noise_init(&wf->noise, 0xFA11);
    ag_biquad_init(&wf->lp1); ag_biquad_set(&wf->lp1, AG_FILTER_LP, 2000.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&wf->lp2); ag_biquad_set(&wf->lp2, AG_FILTER_LP, 600.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&wf->hp); ag_biquad_set(&wf->hp, AG_FILTER_HP, 80.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&wf->roar_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&wf->roar_lfo, 0.12f);
}
float ag_waterfall_next(AgWaterfall *wf) {
    float n = ag_noise_white(&wf->noise)*0.4f + ag_noise_pink(&wf->noise)*0.6f;
    n = ag_biquad_process(&wf->lp1, n);
    float low = ag_biquad_process(&wf->lp2, n);
    float roar = ag_osc_next(&wf->roar_lfo) * wf->roar;
    float out = n*0.6f + low*(0.4f + roar*0.3f);
    out = ag_biquad_process(&wf->hp, out);
    return out * wf->gain;
}

void ag_drip_init(AgDrip *d, double sr) {
    memset(d,0,sizeof(*d));
    d->sr=sr>0?sr:AG_SR_DEFAULT;
    d->gain=0.6f;
    ag_osc_init(&d->osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&d->osc, 1200);
    AgADSR adsr={0.001f,0.15f,0.0f,0.05f,0.5f,1.5f,1.0f};
    ag_env_init(&d->env, adsr, sr);
    ag_biquad_init(&d->filter);
    ag_biquad_set(&d->filter, AG_FILTER_BP, 1200, 2.0f, 0, (float)sr);
    ag_rng_seed(&d->rng, 0xD11A);
    d->next_drip_time=0.5;
}
void ag_drip_trigger(AgDrip *d, float freq, float gain) {
    ag_osc_set_freq(&d->osc, freq);
    ag_biquad_set(&d->filter, AG_FILTER_BP, freq, 2.5f, 0, (float)d->sr);
    ag_env_trigger(&d->env);
    d->active=1;
    d->gain=gain;
    d->drip_freq=freq;
}
float ag_drip_next(AgDrip *d) {
    d->timer += 1.0/d->sr;
    float out=0;
    if(d->active){
        float env=ag_env_next(&d->env);
        if(ag_env_is_idle(&d->env)) d->active=0;
        else {
            float s=ag_osc_next(&d->osc);
            s=ag_biquad_process(&d->filter, s);
            out=s*env*d->gain;
        }
    }
    return out;
}
void ag_drip_auto(AgDrip *d, float density) {
    if(d->timer >= d->next_drip_time){
        d->timer=0;
        float interval = ag_rng_range_f32(&d->rng, 0.2f, 2.0f) / (density>0.01f?density:0.01f);
        d->next_drip_time=interval;
        float freq=ag_rng_range_f32(&d->rng, 800, 2500);
        float gain=ag_rng_range_f32(&d->rng, 0.3f, 0.8f);
        ag_drip_trigger(d,freq,gain);
    }
}

void ag_bubbles_init(AgBubbles *b, double sr) {
    memset(b,0,sizeof(*b));
    b->sr=sr>0?sr:AG_SR_DEFAULT;
    b->gain=0.5f;
    b->rate=3.0f;
    ag_rng_seed(&b->rng, 0xB0B);
    ag_osc_init(&b->osc, AG_OSC_SINE, sr);
    AgADSR adsr={0.005f,0.2f,0.0f,0.05f,1,1,1};
    ag_env_init(&b->env, adsr, sr);
    b->next_bubble=0.3;
}
void ag_bubbles_set_rate(AgBubbles *b, float rate){ b->rate=ag_clamp_f(rate,0.1f,20.0f); }
float ag_bubbles_next(AgBubbles *b) {
    b->timer+=1.0/b->sr;
    if(b->timer >= b->next_bubble){
        b->timer=0;
        b->next_bubble=ag_rng_range_f32(&b->rng, 0.1f, 0.8f) / (b->rate*0.3f+0.1f);
        float freq=ag_rng_range_f32(&b->rng, 300, 1200);
        ag_osc_set_freq(&b->osc, freq);
        ag_env_trigger(&b->env);
        b->active=1;
    }
    if(!b->active) return 0;
    float env=ag_env_next(&b->env);
    if(ag_env_is_idle(&b->env)) b->active=0;
    /* rising pitch */
    float freq_inc = b->osc.freq * 0.0005f;
    ag_osc_set_freq(&b->osc, b->osc.freq + freq_inc);
    float s=ag_osc_next(&b->osc)*env*b->gain;
    return s;
}

void ag_underwater_amb_init(AgUnderwaterAmbience *uw, double sr) {
    memset(uw,0,sizeof(*uw));
    uw->sr=sr>0?sr:AG_SR_DEFAULT;
    uw->gain=0.5f;
    ag_river_init(&uw->base, sr);
    uw->base.gain=0.6f;
    ag_biquad_init(&uw->muffle_lp);
    ag_biquad_set(&uw->muffle_lp, AG_FILTER_LP, 600.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&uw->pressure_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&uw->pressure_lfo, 0.08f);
}
float ag_underwater_amb_next(AgUnderwaterAmbience *uw) {
    float base=ag_river_next(&uw->base);
    base=ag_biquad_process(&uw->muffle_lp, base);
    float press=ag_osc_next(&uw->pressure_lfo)*0.2f;
    float fc=500.0f + press*200.0f;
    ag_biquad_set(&uw->muffle_lp, AG_FILTER_LP, fc, 0.7f, 0, (float)uw->sr);
    return base*uw->gain;
}
