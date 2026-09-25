#ifndef AG_DRUMS_H
#define AG_DRUMS_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_envelope.h"
#include "ag_filter.h"
#include "ag_noise.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AG_DRUM_KICK = 0,
    AG_DRUM_SNARE,
    AG_DRUM_HIHAT_CLOSED,
    AG_DRUM_HIHAT_OPEN,
    AG_DRUM_CLAP,
    AG_DRUM_TOM_LOW,
    AG_DRUM_TOM_MID,
    AG_DRUM_TOM_HIGH,
    AG_DRUM_RIM,
    AG_DRUM_COWBELL,
    AG_DRUM_CYMBAL,
    AG_DRUM_COUNT
} AgDrumType;

typedef struct AgDrumParams {
    AgDrumType type;
    float tune;       /* 0.5..2.0 multiplier */
    float decay;      /* sec */
    float snap;       /* 0..1 attack click */
    float noise_mix;  /* 0..1 */
    float gain;
    uint64_t seed;
} AgDrumParams;

typedef struct AgDrumVoice {
    AgDrumParams params;
    AgOsc osc;
    AgOsc osc2;
    AgNoise noise;
    AgBiquad filter;
    AgEnv env_amp;
    AgEnv env_pitch;
    AgEnv env_noise;
    double t;
    double sr;
    int active;
    /* clap: multiple bursts */
    int clap_burst;
    double clap_timer;
} AgDrumVoice;

void ag_drum_params_default(AgDrumParams *p, AgDrumType type);
void ag_drum_voice_init(AgDrumVoice *v, const AgDrumParams *p, float sr);
int ag_drum_voice_active(const AgDrumVoice *v);
float ag_drum_voice_next(AgDrumVoice *v);

int ag_drum_render(const AgDrumParams *params, float *out, int max_frames, float sr);
int ag_drum_render_stereo(const AgDrumParams *params, float *stereo_interleaved, int max_frames, float sr);

/* Convenience one-shots */
int ag_drum_kick(float *out, int max_frames, float sr, float tune);
int ag_drum_snare(float *out, int max_frames, float sr, float tune);
int ag_drum_hihat(float *out, int max_frames, float sr, int open);
int ag_drum_clap(float *out, int max_frames, float sr);

/* Drum machine pattern */
#define AG_DRUM_PATTERN_STEPS 16
#define AG_DRUM_TRACKS 8

typedef struct AgDrumPattern {
    int steps[AG_DRUM_TRACKS][AG_DRUM_PATTERN_STEPS]; /* 0/1 or velocity 0..127 */
    float swing;  /* 0..0.6 */
    float bpm;
    AgDrumParams kits[AG_DRUM_TRACKS];
} AgDrumPattern;

void ag_drum_pattern_init(AgDrumPattern *pat, float bpm);
void ag_drum_pattern_set_kit(AgDrumPattern *pat, int track, AgDrumType type);
void ag_drum_pattern_set_step(AgDrumPattern *pat, int track, int step, int vel);

typedef struct AgDrumMachine {
    AgDrumPattern pattern;
    AgDrumVoice voices[AG_DRUM_TRACKS];
    double sr;
    double playhead;
    int current_step;
    double next_step_time;
    double step_dur;
    AgRng rng;
} AgDrumMachine;

void ag_drum_machine_init(AgDrumMachine *dm, const AgDrumPattern *pat, float sr);
void ag_drum_machine_trigger_step(AgDrumMachine *dm, int step);
float ag_drum_machine_next(AgDrumMachine *dm); /* mono mix */
void ag_drum_machine_render(AgDrumMachine *dm, float *out, int frames);

#ifdef __cplusplus
}
#endif

#endif
