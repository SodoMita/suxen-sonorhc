#ifndef AG_SAMPLER_H
#define AG_SAMPLER_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Simple sampler with linear interpolation and pitch shifting */

typedef struct AgSample {
    float *data; /* mono */
    int frames;
    int sr;
    int owned; /* if 1, free on destroy */
} AgSample;

typedef struct AgSamplerVoice {
    AgSample *sample;
    double pos;
    double inc;
    float gain;
    float pan_l, pan_r;
    int active;
    int loop;
    int loop_start;
    int loop_end;
} AgSamplerVoice;

void ag_sample_init(AgSample *s);
void ag_sample_alloc(AgSample *s, int frames, int sr);
void ag_sample_free(AgSample *s);
void ag_sample_from_sine(AgSample *s, float freq, float dur, int sr);

void ag_sampler_voice_init(AgSamplerVoice *v, AgSample *sample);
void ag_sampler_voice_play(AgSamplerVoice *v, float pitch_semi, float gain, float pan);
void ag_sampler_voice_stop(AgSamplerVoice *v);
int ag_sampler_voice_active(const AgSamplerVoice *v);
float ag_sampler_voice_next(AgSamplerVoice *v);
void ag_sampler_voice_next_stereo(AgSamplerVoice *v, float *l, float *r);

/* Multi-sample instrument */
#define AG_SAMPLER_MAX_ZONES 16

typedef struct AgSamplerZone {
    AgSample sample;
    int root_midi;
    int low_midi;
    int high_midi;
    float tune_cents;
} AgSamplerZone;

typedef struct AgSamplerInstrument {
    AgSamplerZone zones[AG_SAMPLER_MAX_ZONES];
    int zone_count;
} AgSamplerInstrument;

void ag_sampler_inst_init(AgSamplerInstrument *inst);
void ag_sampler_inst_add_zone(AgSamplerInstrument *inst, const AgSample *sample, int root_midi, int low_midi, int high_midi);
AgSamplerZone* ag_sampler_inst_find_zone(AgSamplerInstrument *inst, int midi);

typedef struct AgSamplerPoly {
    AgSamplerInstrument *inst;
    AgSamplerVoice voices[16];
    int voice_count;
    double sr;
} AgSamplerPoly;

void ag_sampler_poly_init(AgSamplerPoly *poly, AgSamplerInstrument *inst, double sr);
void ag_sampler_poly_note_on(AgSamplerPoly *poly, int midi, float vel);
void ag_sampler_poly_note_off(AgSamplerPoly *poly, int midi);
float ag_sampler_poly_next(AgSamplerPoly *poly);
void ag_sampler_poly_next_stereo(AgSamplerPoly *poly, float *l, float *r);

#ifdef __cplusplus
}
#endif

#endif
