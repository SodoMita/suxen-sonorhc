#include "ag_nature.h"
#include <string.h>
#include <math.h>

void ag_bird_init(AgBird *b, double sr, int species) {
    memset(b,0,sizeof(*b));
    b->sr=sr>0?sr:AG_SR_DEFAULT;
    b->species=species;
    b->gain=0.8f;
    b->base_freq=2000.0f + species*500.0f;
    ag_osc_init(&b->osc, AG_OSC_SINE, sr);
    ag_osc_init(&b->mod, AG_OSC_SINE, sr);
    ag_osc_set_freq(&b->mod, 80);
    AgADSR adsr={0.01f,0.12f,0.0f,0.08f,0.8f,1.2f,1.0f};
    ag_env_init(&b->env, adsr, sr);
    ag_biquad_init(&b->filter);
    ag_biquad_set(&b->filter, AG_FILTER_BP, b->base_freq, 2.0f, 0, (float)sr);
    ag_rng_seed(&b->rng, 0xB11D + species*100);
    b->next_call=1.0;
}
void ag_bird_trigger(AgBird *b) {
    float freq = b->base_freq + ag_rng_range_f32(&b->rng, -200, 400);
    ag_osc_set_freq(&b->osc, freq);
    ag_osc_set_freq(&b->mod, ag_rng_range_f32(&b->rng, 40, 120));
    ag_biquad_set(&b->filter, AG_FILTER_BP, freq, 2.5f, 0, (float)b->sr);
    ag_env_trigger(&b->env);
    b->active=1;
}
float ag_bird_next(AgBird *b) {
    b->timer+=1.0/b->sr;
    if(!b->active) return 0;
    float env=ag_env_next(&b->env);
    if(ag_env_is_idle(&b->env)) b->active=0;
    float mod = ag_osc_next(&b->mod) * 200.0f;
    ag_osc_set_freq(&b->osc, b->base_freq + mod);
    float s = ag_osc_next(&b->osc) * env;
    s = ag_biquad_process(&b->filter, s);
    return s * b->gain;
}
void ag_bird_auto(AgBird *b, float density) {
    if(b->timer >= b->next_call){
        b->timer=0;
        b->next_call = ag_rng_range_f32(&b->rng, 0.5f, 4.0f) / (density>0.01f?density:0.01f);
        if(ag_rng_next_f32(&b->rng) < density) ag_bird_trigger(b);
    }
}

void ag_cricket_init(AgCricket *c, double sr) {
    memset(c,0,sizeof(*c));
    c->sr=sr>0?sr:AG_SR_DEFAULT;
    c->gain=0.5f;
    ag_osc_init(&c->osc1, AG_OSC_SINE, sr);
    ag_osc_init(&c->osc2, AG_OSC_SINE, sr);
    ag_osc_set_freq(&c->osc1, 4500);
    ag_osc_set_freq(&c->osc2, 4600);
    ag_osc_init(&c->am_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&c->am_lfo, 30);
    AgADSR adsr={0.005f,0.08f,0.0f,0.02f,1,1,1};
    ag_env_init(&c->env, adsr, sr);
    ag_rng_seed(&c->rng, 0xC11C);
    c->next_chirp=0.02;
    ag_env_trigger(&c->env);
    c->active=1;
}
float ag_cricket_next(AgCricket *c) {
    c->timer+=1.0/c->sr;
    if(c->timer >= c->next_chirp){
        c->timer=0;
        c->next_chirp = ag_rng_range_f32(&c->rng, 0.1f, 0.4f);
        ag_env_trigger(&c->env);
        c->active=1;
    }
    if(!c->active) return 0;
    float env=ag_env_next(&c->env);
    if(ag_env_is_idle(&c->env)) c->active=0;
    float am = ag_osc_next(&c->am_lfo) * 0.5f + 0.5f;
    float s1=ag_osc_next(&c->osc1);
    float s2=ag_osc_next(&c->osc2);
    float s = (s1+s2)*0.5f * am * env;
    return s * c->gain;
}
void ag_cricket_auto(AgCricket *c, float density) {
    (void)density;
    /* auto handled in next */
}

void ag_cicada_init(AgCicada *c, double sr) {
    memset(c,0,sizeof(*c));
    c->sr=sr>0?sr:AG_SR_DEFAULT;
    c->gain=0.35f;
    ag_noise_init(&c->noise, 0xC1CA);
    ag_biquad_init(&c->bp);
    ag_biquad_set(&c->bp, AG_FILTER_BP, 4000.0f, 3.0f, 0, (float)sr);
    ag_osc_init(&c->am_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&c->am_lfo, 120.0f);
    ag_rng_seed(&c->rng, 0xC1CA);
}
float ag_cicada_next(AgCicada *c) {
    float n = ag_noise_white(&c->noise);
    n = ag_biquad_process(&c->bp, n);
    float am = ag_osc_next(&c->am_lfo) * 0.5f + 0.5f;
    /* slow variation */
    c->timer+=1.0/c->sr;
    if(c->timer>0.5){
        c->timer=0;
        float fc = ag_rng_range_f32(&c->rng, 3500, 5000);
        ag_biquad_set(&c->bp, AG_FILTER_BP, fc, 3.0f, 0, (float)c->sr);
    }
    return n * am * c->gain;
}

void ag_frog_init(AgFrog *f, double sr) {
    memset(f,0,sizeof(*f));
    f->sr=sr>0?sr:AG_SR_DEFAULT;
    f->gain=0.6f;
    ag_osc_init(&f->osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&f->osc, 200);
    ag_biquad_init(&f->formant);
    ag_biquad_set(&f->formant, AG_FILTER_BP, 600.0f, 1.5f, 0, (float)sr);
    AgADSR adsr={0.01f,0.25f,0.0f,0.1f,1,1,1};
    ag_env_init(&f->env, adsr, sr);
    ag_rng_seed(&f->rng, 0xF1109);
    f->next_croak=1.0;
}
float ag_frog_next(AgFrog *f) {
    f->timer+=1.0/f->sr;
    if(f->timer >= f->next_croak){
        f->timer=0;
        f->next_croak = ag_rng_range_f32(&f->rng, 0.8f, 3.0f);
        float freq = ag_rng_range_f32(&f->rng, 150, 350);
        ag_osc_set_freq(&f->osc, freq);
        ag_biquad_set(&f->formant, AG_FILTER_BP, freq*2.5f, 1.2f, 0, (float)f->sr);
        ag_env_trigger(&f->env);
        f->active=1;
    }
    if(!f->active) return 0;
    float env=ag_env_next(&f->env);
    if(ag_env_is_idle(&f->env)) f->active=0;
    float s=ag_osc_next(&f->osc) * env;
    s=ag_biquad_process(&f->formant, s);
    return s * f->gain;
}
void ag_frog_auto(AgFrog *f, float density){ (void)density; }

void ag_swarm_init(AgInsectSwarm *s, double sr) {
    memset(s,0,sizeof(*s));
    s->sr=sr>0?sr:AG_SR_DEFAULT;
    s->gain=0.3f;
    s->density=0.5f;
    ag_rng_seed(&s->rng, 0x5A11A);
    for(int i=0;i<AG_SWARM_MAX;i++){
        ag_osc_init(&s->grains[i].osc, AG_OSC_SINE, sr);
        float freq = ag_rng_range_f32(&s->rng, 3000, 8000);
        ag_osc_set_freq(&s->grains[i].osc, freq);
        s->grains[i].pan = ag_rng_range_f32(&s->rng, -1,1);
        s->grains[i].active=0;
    }
}
float ag_swarm_next(AgInsectSwarm *s) {
    float l,r; ag_swarm_next_stereo(s,&l,&r); return (l+r)*0.5f;
}
void ag_swarm_next_stereo(AgInsectSwarm *s, float *out_l, float *out_r) {
    s->timer+=1.0/s->sr;
    if(s->timer > 0.05f){
        s->timer=0;
        for(int i=0;i<AG_SWARM_MAX;i++){
            if(!s->grains[i].active && ag_rng_next_f32(&s->rng) < s->density*0.2f){
                float freq = ag_rng_range_f32(&s->rng, 3000, 9000);
                ag_osc_set_freq(&s->grains[i].osc, freq);
                s->grains[i].osc.phase = ag_rng_next_f32(&s->rng);
                s->grains[i].active=1;
                s->grains[i].pan = ag_rng_range_f32(&s->rng, -1,1);
                break;
            }
        }
    }
    float ml=0,mr=0;
    for(int i=0;i<AG_SWARM_MAX;i++){
        if(!s->grains[i].active) continue;
        float samp = ag_osc_next(&s->grains[i].osc) * 0.2f;
        /* simple decay */
        samp *= 0.98f; /* not accurate */
        if(fabsf(samp)<0.001f) s->grains[i].active=0;
        float pl,pr; ag_buffer_pan_stereo(samp, s->grains[i].pan, &pl, &pr);
        ml+=pl; mr+=pr;
    }
    *out_l=ml*s->gain;
    *out_r=mr*s->gain;
}

void ag_owl_init(AgOwl *o, double sr) {
    memset(o,0,sizeof(*o));
    o->sr=sr>0?sr:AG_SR_DEFAULT;
    o->gain=0.5f;
    ag_osc_init(&o->osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&o->osc, 400);
    AgADSR adsr={0.05f,0.4f,0.0f,0.2f,1,1,1};
    ag_env_init(&o->env, adsr, sr);
    ag_biquad_init(&o->filter);
    ag_biquad_set(&o->filter, AG_FILTER_LP, 800.0f, 0.7f, 0, (float)sr);
    ag_rng_seed(&o->rng, 0x0A1);
    o->next_hoot=3.0;
}
float ag_owl_next(AgOwl *o) {
    o->timer+=1.0/o->sr;
    if(o->timer >= o->next_hoot){
        o->timer=0;
        o->next_hoot = ag_rng_range_f32(&o->rng, 2.0f, 6.0f);
        ag_env_trigger(&o->env);
        o->active=1;
    }
    if(!o->active) return 0;
    float env=ag_env_next(&o->env);
    if(ag_env_is_idle(&o->env)) o->active=0;
    float s=ag_osc_next(&o->osc)*env;
    s=ag_biquad_process(&o->filter, s);
    return s*o->gain;
}
void ag_owl_auto(AgOwl *o, float density){ (void)density; }
