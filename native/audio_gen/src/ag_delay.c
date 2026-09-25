#include "ag_delay.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void ag_delay_init(AgDelay *d, int sr, float max_delay_ms) {
    memset(d,0,sizeof(*d));
    d->sr = sr>0?sr:AG_SR_DEFAULT;
    d->dry = 0.7f;
    d->wet = 0.3f;
    d->feedback = 0.4f;
    d->delay_ms = 300.0f;
    if (max_delay_ms < 10) max_delay_ms = 2000.0f;
    int sz = (int)(d->sr * max_delay_ms / 1000.0f);
    if (sz<1) sz=1;
    d->buf = (float*)calloc(sz, sizeof(float));
    d->size = sz;
    d->pos = 0;
    d->initialized = 1;
    ag_biquad_init(&d->filter);
    ag_biquad_set(&d->filter, AG_FILTER_LP, 4000.0f, 0.7f, 0, (float)sr);
}
void ag_delay_free(AgDelay *d) {
    if (!d->initialized) return;
    free(d->buf);
    d->initialized=0;
}
void ag_delay_set_delay(AgDelay *d, float delay_ms) {
    d->delay_ms = ag_clamp_f(delay_ms, 1.0f, (float)d->size / d->sr * 1000.0f);
}
void ag_delay_set_feedback(AgDelay *d, float fb) { d->feedback = ag_clamp_f(fb, 0, 0.99f); }
void ag_delay_set_wet(AgDelay *d, float wet) { d->wet = ag_clamp_f(wet,0,1); }
void ag_delay_set_dry(AgDelay *d, float dry) { d->dry = ag_clamp_f(dry,0,1); }
void ag_delay_set_filter(AgDelay *d, float cutoff, float q) {
    d->filter_on = 1;
    ag_biquad_set(&d->filter, AG_FILTER_LP, cutoff, q, 0, (float)d->sr);
}
float ag_delay_process(AgDelay *d, float in) {
    if (!d->initialized) return in;
    int delay_samples = (int)(d->delay_ms * d->sr / 1000.0f);
    if (delay_samples < 1) delay_samples=1;
    if (delay_samples > d->size) delay_samples = d->size;
    int read_pos = d->pos - delay_samples;
    if (read_pos < 0) read_pos += d->size;
    float delayed = d->buf[read_pos];
    if (d->filter_on) delayed = ag_biquad_process(&d->filter, delayed);
    float out = in * d->dry + delayed * d->wet;
    d->buf[d->pos] = in + delayed * d->feedback;
    d->pos = (d->pos+1)%d->size;
    return out;
}
void ag_delay_process_block(AgDelay *d, float *buf, int frames) {
    for (int i=0;i<frames;i++) buf[i]=ag_delay_process(d, buf[i]);
}

/* Ping-pong */
void ag_pingpong_init(AgPingPong *pp, int sr, float max_delay_ms) {
    memset(pp,0,sizeof(*pp));
    ag_delay_init(&pp->left, sr, max_delay_ms);
    ag_delay_init(&pp->right, sr, max_delay_ms);
    pp->cross_feedback = 0.2f;
}
void ag_pingpong_free(AgPingPong *pp) {
    ag_delay_free(&pp->left);
    ag_delay_free(&pp->right);
}
void ag_pingpong_set_delay(AgPingPong *pp, float delay_ms) {
    ag_delay_set_delay(&pp->left, delay_ms);
    ag_delay_set_delay(&pp->right, delay_ms*1.5f);
}
void ag_pingpong_set_feedback(AgPingPong *pp, float fb) {
    ag_delay_set_feedback(&pp->left, fb);
    ag_delay_set_feedback(&pp->right, fb);
}
void ag_pingpong_set_cross(AgPingPong *pp, float cross) { pp->cross_feedback = ag_clamp_f(cross,0,0.9f); }
void ag_pingpong_process(AgPingPong *pp, float in_l, float in_r, float *out_l, float *out_r) {
    /* simple cross */
    float l_del = ag_delay_process(&pp->left, in_l + in_r * pp->cross_feedback * 0.5f);
    float r_del = ag_delay_process(&pp->right, in_r + in_l * pp->cross_feedback * 0.5f);
    *out_l = l_del;
    *out_r = r_del;
}

/* Tape */
void ag_tape_delay_init(AgTapeDelay *td, int sr, float max_delay_ms) {
    memset(td,0,sizeof(*td));
    ag_delay_init(&td->delay, sr, max_delay_ms);
    ag_osc_init(&td->wow_osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&td->wow_osc, 0.5f);
    ag_osc_init(&td->flutter_osc, AG_OSC_SINE, sr);
    ag_osc_set_freq(&td->flutter_osc, 6.0f);
    td->wow_depth = 0.002f;
    td->flutter_depth = 0.001f;
}
void ag_tape_delay_free(AgTapeDelay *td) { ag_delay_free(&td->delay); }
void ag_tape_delay_set(AgTapeDelay *td, float delay_ms, float fb, float wow, float flutter) {
    ag_delay_set_delay(&td->delay, delay_ms);
    ag_delay_set_feedback(&td->delay, fb);
    td->wow_depth = wow;
    td->flutter_depth = flutter;
}
float ag_tape_delay_process(AgTapeDelay *td, float in) {
    float wow = ag_osc_next(&td->wow_osc) * td->wow_depth;
    float flutter = ag_osc_next(&td->flutter_osc) * td->flutter_depth;
    float mod = 1.0f + wow + flutter;
    td->delay.delay_ms *= mod;
    float out = ag_delay_process(&td->delay, in);
    td->delay.delay_ms /= mod;
    return out;
}
