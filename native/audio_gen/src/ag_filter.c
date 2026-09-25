#include "ag_filter.h"
#include <math.h>
#include <string.h>

void ag_biquad_init(AgBiquad *f) {
    memset(f,0,sizeof(*f));
    f->b0=1.0f;
}

void ag_biquad_set(AgBiquad *f, AgFilterType type, float freq, float q, float gain_db, float sr) {
    if (sr < 100) sr = 44100;
    if (freq < 1) freq = 1;
    if (freq > sr*0.48f) freq = sr*0.48f;
    if (q < 0.01f) q = 0.01f;
    float w0 = 2.0f * (float)AG_PI * freq / sr;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * q);
    float A = powf(10.0f, gain_db / 40.0f);
    float b0=1,b1=0,b2=0,a0=1,a1=0,a2=0;
    switch (type) {
        case AG_FILTER_LP:
            b0 = (1 - cos_w0) * 0.5f;
            b1 = 1 - cos_w0;
            b2 = b0;
            a0 = 1 + alpha;
            a1 = -2*cos_w0;
            a2 = 1 - alpha;
            break;
        case AG_FILTER_HP:
            b0 = (1 + cos_w0)*0.5f;
            b1 = -(1 + cos_w0);
            b2 = b0;
            a0 = 1 + alpha;
            a1 = -2*cos_w0;
            a2 = 1 - alpha;
            break;
        case AG_FILTER_BP:
            b0 = sin_w0*0.5f;
            b1 = 0;
            b2 = -sin_w0*0.5f;
            a0 = 1 + alpha;
            a1 = -2*cos_w0;
            a2 = 1 - alpha;
            break;
        case AG_FILTER_NOTCH:
            b0 = 1;
            b1 = -2*cos_w0;
            b2 = 1;
            a0 = 1 + alpha;
            a1 = -2*cos_w0;
            a2 = 1 - alpha;
            break;
        case AG_FILTER_PEAK: {
            float alphaA = alpha * A;
            float alphaDiv = alpha / A;
            b0 = 1 + alphaA;
            b1 = -2*cos_w0;
            b2 = 1 - alphaA;
            a0 = 1 + alphaDiv;
            a1 = -2*cos_w0;
            a2 = 1 - alphaDiv;
        } break;
        case AG_FILTER_LOSHELF: {
            float c = (A+1.0f/A)*(1.0f/q -1.0f) + 2.0f;
            /* simplified shelf */
            b0 = A*((A+1)-(A-1)*cos_w0 + sqrtf(A)*sin_w0*sqrtf(c));
            b1 = 2*A*((A-1)-(A+1)*cos_w0);
            b2 = A*((A+1)-(A-1)*cos_w0 - sqrtf(A)*sin_w0*sqrtf(c));
            a0 = (A+1)+(A-1)*cos_w0 + sqrtf(A)*sin_w0*sqrtf(c);
            a1 = -2*((A-1)+(A+1)*cos_w0);
            a2 = (A+1)+(A-1)*cos_w0 - sqrtf(A)*sin_w0*sqrtf(c);
        } break;
        case AG_FILTER_HISHELF: {
            float c = (A+1.0f/A)*(1.0f/q -1.0f) + 2.0f;
            b0 = A*((A+1)+(A-1)*cos_w0 + sqrtf(A)*sin_w0*sqrtf(c));
            b1 = -2*A*((A-1)+(A+1)*cos_w0);
            b2 = A*((A+1)+(A-1)*cos_w0 - sqrtf(A)*sin_w0*sqrtf(c));
            a0 = (A+1)-(A-1)*cos_w0 + sqrtf(A)*sin_w0*sqrtf(c);
            a1 = 2*((A-1)-(A+1)*cos_w0);
            a2 = (A+1)-(A-1)*cos_w0 - sqrtf(A)*sin_w0*sqrtf(c);
        } break;
        default: break;
    }
    f->b0 = b0 / a0;
    f->b1 = b1 / a0;
    f->b2 = b2 / a0;
    f->a1 = a1 / a0;
    f->a2 = a2 / a0;
}

float ag_biquad_process(AgBiquad *f, float in) {
    float out = f->b0*in + f->z1;
    f->z1 = f->b1*in - f->a1*out + f->z2;
    f->z2 = f->b2*in - f->a2*out;
    return out;
}
void ag_biquad_process_block(AgBiquad *f, float *buf, int n) {
    for (int i=0;i<n;i++) buf[i]=ag_biquad_process(f, buf[i]);
}

/* One pole */
void ag_onepole_lp(AgOnePole *f, float freq, float sr) {
    float x = expf(-2.0f * (float)AG_PI * freq / sr);
    f->b1 = x;
    f->a0 = 1.0f - x;
    f->z1 = 0.0f;
}
void ag_onepole_hp(AgOnePole *f, float freq, float sr) {
    float x = expf(-2.0f * (float)AG_PI * freq / sr);
    f->b1 = x;
    f->a0 = 1.0f - x;
    f->z1 = 0.0f;
}
float ag_onepole_process(AgOnePole *f, float in) {
    float out = f->a0*in + f->b1*f->z1;
    f->z1 = out;
    return out;
}

/* SVF */
void ag_svf_init(AgSVF *svf, float freq, float q, float sr) {
    memset(svf,0,sizeof(*svf));
    svf->sr = sr;
    ag_svf_set(svf,freq,q);
}
void ag_svf_set(AgSVF *svf, float freq, float q) {
    svf->freq = freq;
    svf->q = q < 0.1f ? 0.1f : q;
}
void ag_svf_process(AgSVF *svf, float in, float *lp, float *hp, float *bp, float *notch) {
    float g = tanf((float)AG_PI * svf->freq / svf->sr);
    float k = 1.0f / svf->q;
    float a1 = 1.0f / (1.0f + g*(g+k));
    float a2 = g*a1;
    float a3 = g*a2;
    float v3 = in - svf->ic2eq;
    float v1 = a1*svf->ic1eq + a2*v3;
    float v2 = svf->ic2eq + a2*svf->ic1eq + a3*v3;
    svf->ic1eq = 2*v1 - svf->ic1eq;
    svf->ic2eq = 2*v2 - svf->ic2eq;
    if (lp) *lp = v2;
    if (bp) *bp = v1;
    if (hp) *hp = in - k*v1 - v2;
    if (notch) *notch = in - k*v1;
}

/* DC blocker */
void ag_dcblock_init(AgDCBlock *f) { f->x1=0; f->y1=0; }
float ag_dcblock_process(AgDCBlock *f, float in) {
    float y = in - f->x1 + 0.995f * f->y1;
    f->x1 = in;
    f->y1 = y;
    return y;
}

/* Ladder */
void ag_ladder_init(AgLadder *f, float freq, float res, float sr) {
    memset(f,0,sizeof(*f));
    f->sr = sr;
    ag_ladder_set(f,freq,res);
}
void ag_ladder_set(AgLadder *f, float freq, float res) {
    f->freq = freq;
    f->res = ag_clamp_f(res, 0.0f, 0.99f);
}
float ag_ladder_process(AgLadder *f, float in) {
    float g = 2.0f * (float)AG_PI * f->freq / f->sr;
    if (g>1.0f) g=1.0f;
    float res = f->res * 4.0f;
    float input = in - res * f->stage[3];
    for (int i=0;i<4;i++) {
        float stage_in = input;
        if (i>0) stage_in = f->stage[i-1];
        f->stage[i] = f->stage[i] + g * (tanhf(stage_in) - tanhf(f->stage[i]));
    }
    return f->stage[3];
}
