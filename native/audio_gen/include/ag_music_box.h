#ifndef AG_MUSIC_BOX_H
#define AG_MUSIC_BOX_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_envelope.h"
#include "ag_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Karplus-Strong plucked string */
#define AG_KS_MAX_DELAY 4096

typedef struct AgKarplus {
    float buf[AG_KS_MAX_DELAY];
    int buf_len;
    int pos;
    float feedback;
    float damping;
    AgBiquad filter;
    double sr;
    int active;
    float gain;
} AgKarplus;

void ag_ks_init(AgKarplus *ks, double sr);
void ag_ks_pluck(AgKarplus *ks, float freq, float damping, float gain);
float ag_ks_next(AgKarplus *ks);
int ag_ks_active(const AgKarplus *ks);

/* Music box tine - damped sine + overtone */
typedef struct AgTine {
    AgOsc osc;
    AgOsc overtone;
    AgEnv env;
    AgBiquad filter;
    double sr;
    int active;
    float gain;
} AgTine;

void ag_tine_init(AgTine *t, double sr);
void ag_tine_hit(AgTine *t, float freq, float vel);
float ag_tine_next(AgTine *t);
int ag_tine_active(const AgTine *t);

/* Bell - sum of inharmonic partials */
#define AG_BELL_PARTIALS 8

typedef struct AgBell {
    AgOsc partials[AG_BELL_PARTIALS];
    float amps[AG_BELL_PARTIALS];
    float decays[AG_BELL_PARTIALS];
    float envs[AG_BELL_PARTIALS];
    double sr;
    int active;
    float gain;
    double t;
} AgBell;

void ag_bell_init(AgBell *b, double sr);
void ag_bell_hit(AgBell *b, float freq, float vel);
float ag_bell_next(AgBell *b);
int ag_bell_active(const AgBell *b);

/* Kalimba - similar to tine but with more wood */
typedef struct AgKalimba {
    AgTine tine;
    AgKarplus wood;
    float mix;
} AgKalimba;

void ag_kalimba_init(AgKalimba *k, double sr);
void ag_kalimba_hit(AgKalimba *k, float freq, float vel);
float ag_kalimba_next(AgKalimba *k);

/* Music box sequencer - plays a melody on tines */
#define AG_MUSICBOX_VOICES 16
#define AG_MUSICBOX_NOTES 128

typedef struct AgMusicBoxNote {
    float time; /* sec */
    int midi;
    float vel;
} AgMusicBoxNote;

typedef struct AgMusicBox {
    AgTine voices[AG_MUSICBOX_VOICES];
    AgMusicBoxNote notes[AG_MUSICBOX_NOTES];
    int note_count;
    double sr;
    double playhead;
    int next_note;
    float gain;
    int looping;
} AgMusicBox;

void ag_musicbox_init(AgMusicBox *mb, double sr);
void ag_musicbox_add_note(AgMusicBox *mb, float time, int midi, float vel);
void ag_musicbox_clear(AgMusicBox *mb);
void ag_musicbox_set_loop(AgMusicBox *mb, int loop);
float ag_musicbox_next(AgMusicBox *mb);
void ag_musicbox_render(AgMusicBox *mb, float *out, int frames);

#ifdef __cplusplus
}
#endif

#endif
