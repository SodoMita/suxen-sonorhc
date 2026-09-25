#ifndef AG_OSC_H
#define AG_OSC_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AG_OSC_SINE = 0,
    AG_OSC_SAW,
    AG_OSC_SQUARE,
    AG_OSC_TRI,
    AG_OSC_NOISE,
    AG_OSC_PULSE,
    AG_OSC_SAW_TRI, /* morph */
    AG_OSC_COUNT
} AgOscType;

typedef struct AgOsc {
    double phase;      /* 0..1 */
    double freq;
    double sr;
    int type;
    double pw;         /* pulse width 0..1 for square/pulse */
    double detune;     /* cents */
    AgRng rng;         /* for noise */
    double last_out;   /* for polyBLEP state */
} AgOsc;

void ag_osc_init(AgOsc *osc, AgOscType type, double sr);
void ag_osc_set_freq(AgOsc *osc, double freq);
void ag_osc_set_type(AgOsc *osc, AgOscType type);
void ag_osc_set_pw(AgOsc *osc, double pw);
void ag_osc_reset(AgOsc *osc);

/* Raw (non-bandlimited) - fast */
float ag_osc_next_raw(AgOsc *osc);
/* Bandlimited using polyBLEP for saw/square */
float ag_osc_next(AgOsc *osc);

/* Stateless helpers */
float ag_osc_sine_f(float phase01);
float ag_osc_saw_f(float phase01);
float ag_osc_square_f(float phase01, float pw);
float ag_osc_tri_f(float phase01);

/* PolyBLEP */
float ag_poly_blep(float t, float dt);

/* Wavetable osc - 1024 samples */
#define AG_WT_SIZE 1024
typedef struct AgWavetable {
    float table[AG_WT_SIZE];
} AgWavetable;

void ag_wt_sine(AgWavetable *wt);
void ag_wt_saw(AgWavetable *wt);
void ag_wt_square(AgWavetable *wt, float pw);
void ag_wt_tri(AgWavetable *wt);
void ag_wt_morph(AgWavetable *out, const AgWavetable *a, const AgWavetable *b, float mix);

typedef struct AgWTOsc {
    AgWavetable wt;
    double phase;
    double freq;
    double sr;
} AgWTOsc;

void ag_wt_osc_init(AgWTOsc *osc, const AgWavetable *wt, double sr);
float ag_wt_osc_next(AgWTOsc *osc);

/* LFO */
typedef struct AgLFO {
    AgOsc osc;
    float depth;
    float offset;
} AgLFO;

void ag_lfo_init(AgLFO *lfo, AgOscType type, double freq, double sr, float depth);
float ag_lfo_next(AgLFO *lfo);

#ifdef __cplusplus
}
#endif

#endif
