#include "ag_reverb.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Freeverb tunings at 44100 */
static int comb_tunings[AG_REVERB_COMBS] = {1116,1188,1277,1356,1422,1491,1557,1617};
static int allp_tunings[AG_REVERB_ALLP] = {556,441,341,225};

void ag_reverb_init(AgReverb *rv, int sr) {
    memset(rv,0,sizeof(*rv));
    rv->sr = sr>0?sr:AG_SR_DEFAULT;
    rv->room_size = 0.5f;
    rv->damping = 0.5f;
    rv->wet = 0.33f;
    rv->dry = 0.0f;
    rv->width = 1.0f;
    rv->gain = 0.9f;
    double scale = (double)rv->sr / 44100.0;
    for (int i=0;i<AG_REVERB_COMBS;i++) {
        int sz = (int)(comb_tunings[i]*scale);
        if (sz<10) sz=10;
        rv->comb_bufs[i] = (float*)calloc(sz, sizeof(float));
        rv->combs[i].buf = rv->comb_bufs[i];
        rv->combs[i].size = sz;
        rv->combs[i].feedback = 0.84f;
        rv->combs[i].damp = 0.2f;
    }
    for (int i=0;i<AG_REVERB_ALLP;i++) {
        int sz = (int)(allp_tunings[i]*scale);
        if (sz<10) sz=10;
        rv->allp_bufs[i] = (float*)calloc(sz, sizeof(float));
        rv->allps[i].buf = rv->allp_bufs[i];
        rv->allps[i].size = sz;
        rv->allps[i].feedback = 0.5f;
    }
    rv->initialized=1;
    ag_reverb_set_room_size(rv, rv->room_size);
    ag_reverb_set_damping(rv, rv->damping);
}
void ag_reverb_free(AgReverb *rv) {
    if (!rv->initialized) return;
    for (int i=0;i<AG_REVERB_COMBS;i++) free(rv->comb_bufs[i]);
    for (int i=0;i<AG_REVERB_ALLP;i++) free(rv->allp_bufs[i]);
    rv->initialized=0;
}
void ag_reverb_set_room_size(AgReverb *rv, float room_size) {
    rv->room_size = ag_clamp_f(room_size,0,1);
    for (int i=0;i<AG_REVERB_COMBS;i++) rv->combs[i].feedback = 0.28f + rv->room_size*0.7f;
}
void ag_reverb_set_damping(AgReverb *rv, float damping) {
    rv->damping = ag_clamp_f(damping,0,1);
    for (int i=0;i<AG_REVERB_COMBS;i++) rv->combs[i].damp = rv->damping * 0.4f;
}
void ag_reverb_set_wet(AgReverb *rv, float wet) { rv->wet = ag_clamp_f(wet,0,1); }
void ag_reverb_set_dry(AgReverb *rv, float dry) { rv->dry = ag_clamp_f(dry,0,1); }
void ag_reverb_set_width(AgReverb *rv, float width) { rv->width = ag_clamp_f(width,0,1); }
void ag_reverb_set_gain(AgReverb *rv, float gain) { rv->gain = gain; }

static float comb_process(AgComb *c, float in) {
    float out = c->buf[c->pos];
    c->filter_store = out * (1.0f - c->damp) + c->filter_store * c->damp;
    c->buf[c->pos] = in + c->filter_store * c->feedback;
    c->pos = (c->pos+1)%c->size;
    return out;
}
static float allp_process(AgAllpass *a, float in) {
    float bufout = a->buf[a->pos];
    float out = -in + bufout;
    a->buf[a->pos] = in + bufout * a->feedback;
    a->pos = (a->pos+1)%a->size;
    return out;
}

float ag_reverb_process(AgReverb *rv, float in) {
    if (!rv->initialized) return in;
    float out=0.0f;
    for (int i=0;i<AG_REVERB_COMBS;i++) out += comb_process(&rv->combs[i], in);
    out *= 0.25f;
    for (int i=0;i<AG_REVERB_ALLP;i++) out = allp_process(&rv->allps[i], out);
    return out * rv->gain;
}
void ag_reverb_process_stereo(AgReverb *rv, float in_l, float in_r, float *out_l, float *out_r) {
    float mono = (in_l + in_r)*0.5f;
    float rev = ag_reverb_process(rv, mono);
    float wet1 = rv->wet * (1.0f + rv->width*0.5f);
    float wet2 = rv->wet * (1.0f - rv->width*0.5f);
    *out_l = in_l * rv->dry + rev * wet1;
    *out_r = in_r * rv->dry + rev * wet2;
}
void ag_reverb_process_block(AgReverb *rv, float *buf, int frames) {
    for (int i=0;i<frames;i++) {
        float in = buf[i];
        float rev = ag_reverb_process(rv, in);
        buf[i] = in*rv->dry + rev*rv->wet;
    }
}
void ag_reverb_process_block_stereo(AgReverb *rv, float *interleaved, int frames) {
    for (int i=0;i<frames;i++) {
        float l = interleaved[i*2];
        float r = interleaved[i*2+1];
        float ol,or_;
        ag_reverb_process_stereo(rv,l,r,&ol,&or_);
        interleaved[i*2]=ol;
        interleaved[i*2+1]=or_;
    }
}

/* Schroeder */
void ag_schroeder_init(AgSchroeder *rv, int sr) {
    memset(rv,0,sizeof(*rv));
    rv->sr = sr>0?sr:AG_SR_DEFAULT;
    rv->wet=0.3f; rv->dry=0.7f;
    int combs[4]={1557,1617,1491,1422};
    int allps[2]={225,556};
    double scale = (double)rv->sr/44100.0;
    for (int i=0;i<4;i++) {
        int sz = (int)(combs[i]*scale);
        if (sz<10) sz=10;
        rv->comb_bufs[i]=(float*)calloc(sz,sizeof(float));
        rv->comb_sizes[i]=sz;
        rv->comb_fb[i]=0.84f;
    }
    for (int i=0;i<2;i++) {
        int sz=(int)(allps[i]*scale);
        if (sz<10) sz=10;
        rv->allp_bufs[i]=(float*)calloc(sz,sizeof(float));
        rv->allp_sizes[i]=sz;
    }
    rv->initialized=1;
}
void ag_schroeder_free(AgSchroeder *rv) {
    if (!rv->initialized) return;
    for (int i=0;i<4;i++) free(rv->comb_bufs[i]);
    for (int i=0;i<2;i++) free(rv->allp_bufs[i]);
    rv->initialized=0;
}
float ag_schroeder_process(AgSchroeder *rv, float in) {
    if (!rv->initialized) return in;
    float out=0;
    for (int i=0;i<4;i++) {
        float *buf=rv->comb_bufs[i];
        int pos=rv->comb_pos[i];
        float o=buf[pos];
        buf[pos]=in + o*rv->comb_fb[i];
        rv->comb_pos[i]=(pos+1)%rv->comb_sizes[i];
        out+=o;
    }
    out*=0.25f;
    for (int i=0;i<2;i++) {
        float *buf=rv->allp_bufs[i];
        int pos=rv->allp_pos[i];
        float bufout=buf[pos];
        float o = -out + bufout;
        buf[pos]=out + bufout*0.5f;
        rv->allp_pos[i]=(pos+1)%rv->allp_sizes[i];
        out=o;
    }
    return out*rv->wet + in*rv->dry;
}
