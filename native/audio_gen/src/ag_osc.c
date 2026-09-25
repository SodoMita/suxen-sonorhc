#include "ag_osc.h"
#include <math.h>
#include <string.h>

void ag_osc_init(AgOsc *osc, AgOscType type, double sr) {
    memset(osc, 0, sizeof(*osc));
    osc->type = type;
    osc->sr = sr > 0 ? sr : AG_SR_DEFAULT;
    osc->freq = 440.0;
    osc->pw = 0.5;
    osc->phase = 0.0;
    ag_rng_seed(&osc->rng, 1);
}
void ag_osc_set_freq(AgOsc *osc, double freq) {
    if (freq < 0.1) freq = 0.1;
    if (freq > osc->sr * 0.48) freq = osc->sr * 0.48;
    osc->freq = freq;
}
void ag_osc_set_type(AgOsc *osc, AgOscType type) { osc->type = type; }
void ag_osc_set_pw(AgOsc *osc, double pw) { osc->pw = ag_clamp_d(pw, 0.01, 0.99); }
void ag_osc_reset(AgOsc *osc) { osc->phase = 0.0; osc->last_out = 0.0; }

float ag_osc_sine_f(float phase01) {
    return sinf(phase01 * (float)AG_TAU);
}
float ag_osc_saw_f(float phase01) {
    return phase01 * 2.0f - 1.0f;
}
float ag_osc_square_f(float phase01, float pw) {
    return phase01 < pw ? 1.0f : -1.0f;
}
float ag_osc_tri_f(float phase01) {
    float v = phase01 < 0.5f ? phase01 * 4.0f - 1.0f : 3.0f - phase01 * 4.0f;
    return v;
}
float ag_poly_blep(float t, float dt) {
    if (t < dt) {
        t = t / dt;
        return t + t - t * t - 1.0f;
    } else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

float ag_osc_next_raw(AgOsc *osc) {
    double ph = osc->phase;
    float out = 0.0f;
    switch (osc->type) {
        case AG_OSC_SINE: out = sinf((float)(ph * AG_TAU)); break;
        case AG_OSC_SAW: out = (float)(ph * 2.0 - 1.0); break;
        case AG_OSC_SQUARE: out = ph < osc->pw ? 1.0f : -1.0f; break;
        case AG_OSC_TRI: {
            double p = ph;
            out = p < 0.5 ? (float)(p * 4.0 - 1.0) : (float)(3.0 - p * 4.0);
        } break;
        case AG_OSC_NOISE: out = ag_rng_range_f32(&osc->rng, -1.0f, 1.0f); break;
        case AG_OSC_PULSE: out = ph < osc->pw ? 1.0f : -1.0f; break;
        case AG_OSC_SAW_TRI: {
            float saw = (float)(ph * 2.0 - 1.0);
            float tri = ph < 0.5 ? (float)(ph * 4.0 - 1.0) : (float)(3.0 - ph * 4.0);
            out = saw * 0.5f + tri * 0.5f;
        } break;
        default: out = sinf((float)(ph * AG_TAU)); break;
    }
    osc->phase += osc->freq / osc->sr;
    if (osc->phase >= 1.0) osc->phase -= 1.0;
    return out;
}

float ag_osc_next(AgOsc *osc) {
    if (osc->type == AG_OSC_SINE || osc->type == AG_OSC_NOISE) {
        return ag_osc_next_raw(osc);
    }
    double ph = osc->phase;
    double dt = osc->freq / osc->sr;
    float out = 0.0f;
    switch (osc->type) {
        case AG_OSC_SAW: {
            out = (float)(ph * 2.0 - 1.0);
            out -= ag_poly_blep((float)ph, (float)dt);
        } break;
        case AG_OSC_SQUARE:
        case AG_OSC_PULSE: {
            out = ph < osc->pw ? 1.0f : -1.0f;
            out += ag_poly_blep((float)ph, (float)dt);
            double ph2 = ph - osc->pw;
            if (ph2 < 0) ph2 += 1.0;
            out -= ag_poly_blep((float)ph2, (float)dt);
        } break;
        case AG_OSC_TRI: {
            /* Tri is integrated square, approximate BLEP via 2x */
            float sq = ph < 0.5 ? 1.0f : -1.0f;
            /* naive tri for now, good enough */
            out = ph < 0.5 ? (float)(ph * 4.0 - 1.0) : (float)(3.0 - ph * 4.0);
            (void)sq;
        } break;
        default: out = ag_osc_next_raw(osc); return out;
    }
    osc->phase += dt;
    if (osc->phase >= 1.0) osc->phase -= 1.0;
    return out;
}

/* Wavetable */
void ag_wt_sine(AgWavetable *wt) {
    for (int i=0;i<AG_WT_SIZE;i++) wt->table[i] = sinf((float)i / AG_WT_SIZE * (float)AG_TAU);
}
void ag_wt_saw(AgWavetable *wt) {
    for (int i=0;i<AG_WT_SIZE;i++) wt->table[i] = (float)i / AG_WT_SIZE * 2.0f - 1.0f;
}
void ag_wt_square(AgWavetable *wt, float pw) {
    for (int i=0;i<AG_WT_SIZE;i++) wt->table[i] = ((float)i / AG_WT_SIZE) < pw ? 1.0f : -1.0f;
}
void ag_wt_tri(AgWavetable *wt) {
    for (int i=0;i<AG_WT_SIZE;i++) {
        float ph = (float)i / AG_WT_SIZE;
        wt->table[i] = ph < 0.5f ? ph*4.0f-1.0f : 3.0f - ph*4.0f;
    }
}
void ag_wt_morph(AgWavetable *out, const AgWavetable *a, const AgWavetable *b, float mix) {
    mix = ag_clamp_f(mix, 0.0f, 1.0f);
    for (int i=0;i<AG_WT_SIZE;i++) out->table[i] = a->table[i]*(1.0f-mix) + b->table[i]*mix;
}
void ag_wt_osc_init(AgWTOsc *osc, const AgWavetable *wt, double sr) {
    memset(osc,0,sizeof(*osc));
    if (wt) osc->wt = *wt; else ag_wt_sine(&osc->wt);
    osc->sr = sr>0?sr:AG_SR_DEFAULT;
    osc->freq = 440.0;
    osc->phase = 0.0;
}
float ag_wt_osc_next(AgWTOsc *osc) {
    double ph = osc->phase * AG_WT_SIZE;
    int i0 = (int)ph;
    int i1 = (i0+1) & (AG_WT_SIZE-1);
    float frac = (float)(ph - (double)i0);
    float a = osc->wt.table[i0 & (AG_WT_SIZE-1)];
    float b = osc->wt.table[i1];
    float out = a + (b-a)*frac;
    osc->phase += osc->freq / osc->sr;
    if (osc->phase >= 1.0) osc->phase -= 1.0;
    return out;
}

void ag_lfo_init(AgLFO *lfo, AgOscType type, double freq, double sr, float depth) {
    ag_osc_init(&lfo->osc, type, sr);
    ag_osc_set_freq(&lfo->osc, freq);
    lfo->depth = depth;
    lfo->offset = 0.0f;
}
float ag_lfo_next(AgLFO *lfo) {
    float v = ag_osc_next(&lfo->osc);
    return lfo->offset + v * lfo->depth;
}
