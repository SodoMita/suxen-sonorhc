#include "ag_sampler.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void ag_sample_init(AgSample *s) { memset(s,0,sizeof(*s)); }
void ag_sample_alloc(AgSample *s, int frames, int sr) {
    ag_sample_free(s);
    s->data = (float*)calloc(frames, sizeof(float));
    s->frames = frames;
    s->sr = sr;
    s->owned = 1;
}
void ag_sample_free(AgSample *s) {
    if(s->owned && s->data) free(s->data);
    memset(s,0,sizeof(*s));
}
void ag_sample_from_sine(AgSample *s, float freq, float dur, int sr) {
    int frames = (int)(dur * sr);
    ag_sample_alloc(s, frames, sr);
    for(int i=0;i<frames;i++) s->data[i] = sinf(2.0f*(float)AG_PI*freq*i/sr) * 0.5f;
}

void ag_sampler_voice_init(AgSamplerVoice *v, AgSample *sample) {
    memset(v,0,sizeof(*v));
    v->sample=sample;
    v->pan_l=0.707f; v->pan_r=0.707f;
}
void ag_sampler_voice_play(AgSamplerVoice *v, float pitch_semi, float gain, float pan) {
    v->pos=0;
    v->inc = powf(2.0f, pitch_semi/12.0f);
    v->gain=gain;
    ag_buffer_pan_stereo(1.0f, pan, &v->pan_l, &v->pan_r);
    v->active=1;
}
void ag_sampler_voice_stop(AgSamplerVoice *v) { v->active=0; }
int ag_sampler_voice_active(const AgSamplerVoice *v) { return v->active; }
float ag_sampler_voice_next(AgSamplerVoice *v) {
    if(!v->active || !v->sample || !v->sample->data) return 0.0f;
    int idx = (int)v->pos;
    float frac = (float)(v->pos - idx);
    if(idx<0 || idx>=v->sample->frames) {
        if(v->loop){
            v->pos = v->loop_start;
            idx = v->loop_start;
        } else { v->active=0; return 0.0f; }
    }
    float a = v->sample->data[idx];
    float b = (idx+1 < v->sample->frames) ? v->sample->data[idx+1] : 0.0f;
    float out = a + (b-a)*frac;
    v->pos += v->inc;
    if(v->pos >= v->sample->frames){
        if(v->loop) v->pos = v->loop_start + (v->pos - v->sample->frames);
        else v->active=0;
    }
    return out * v->gain;
}
void ag_sampler_voice_next_stereo(AgSamplerVoice *v, float *l, float *r) {
    float s = ag_sampler_voice_next(v);
    *l = s * v->pan_l;
    *r = s * v->pan_r;
}

void ag_sampler_inst_init(AgSamplerInstrument *inst) { memset(inst,0,sizeof(*inst)); }
void ag_sampler_inst_add_zone(AgSamplerInstrument *inst, const AgSample *sample, int root_midi, int low_midi, int high_midi) {
    if(inst->zone_count >= AG_SAMPLER_MAX_ZONES) return;
    AgSamplerZone *z = &inst->zones[inst->zone_count++];
    z->sample = *sample;
    z->sample.owned=0; /* shallow copy, not owned */
    z->root_midi=root_midi;
    z->low_midi=low_midi;
    z->high_midi=high_midi;
    z->tune_cents=0;
}
AgSamplerZone* ag_sampler_inst_find_zone(AgSamplerInstrument *inst, int midi) {
    for(int i=0;i<inst->zone_count;i++){
        if(midi >= inst->zones[i].low_midi && midi <= inst->zones[i].high_midi) return &inst->zones[i];
    }
    return inst->zone_count>0 ? &inst->zones[0] : NULL;
}

void ag_sampler_poly_init(AgSamplerPoly *poly, AgSamplerInstrument *inst, double sr) {
    memset(poly,0,sizeof(*poly));
    poly->inst=inst;
    poly->sr=sr>0?sr:AG_SR_DEFAULT;
    poly->voice_count=16;
    for(int i=0;i<poly->voice_count;i++) ag_sampler_voice_init(&poly->voices[i], NULL);
}
void ag_sampler_poly_note_on(AgSamplerPoly *poly, int midi, float vel) {
    AgSamplerZone *z = ag_sampler_inst_find_zone(poly->inst, midi);
    if(!z) return;
    /* find free voice */
    AgSamplerVoice *v=NULL;
    for(int i=0;i<poly->voice_count;i++) if(!poly->voices[i].active){ v=&poly->voices[i]; break; }
    if(!v) v=&poly->voices[0];
    v->sample=&z->sample;
    float semi = (midi - z->root_midi) + z->tune_cents/100.0f;
    ag_sampler_voice_play(v, semi, vel, 0.0f);
}
void ag_sampler_poly_note_off(AgSamplerPoly *poly, int midi) {
    (void)midi;
    /* simple: stop all? For now stop first active */
    for(int i=0;i<poly->voice_count;i++) if(poly->voices[i].active){ poly->voices[i].active=0; break; }
}
float ag_sampler_poly_next(AgSamplerPoly *poly) {
    float mix=0;
    for(int i=0;i<poly->voice_count;i++) if(poly->voices[i].active) mix+=ag_sampler_voice_next(&poly->voices[i]);
    return mix;
}
void ag_sampler_poly_next_stereo(AgSamplerPoly *poly, float *l, float *r) {
    float ml=0,mr=0;
    for(int i=0;i<poly->voice_count;i++) if(poly->voices[i].active){
        float sl,sr; ag_sampler_voice_next_stereo(&poly->voices[i], &sl, &sr);
        ml+=sl; mr+=sr;
    }
    *l=ml; *r=mr;
}
