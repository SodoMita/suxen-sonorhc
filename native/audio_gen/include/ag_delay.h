#ifndef AG_DELAY_H
#define AG_DELAY_H

#include "ag_common.h"
#include "ag_filter.h"
#include "ag_osc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AgDelay {
    float *buf;
    int size;
    int pos;
    int sr;
    float delay_ms;
    float feedback;
    float wet;
    float dry;
    AgBiquad filter;
    int filter_on;
    int initialized;
} AgDelay;

void ag_delay_init(AgDelay *d, int sr, float max_delay_ms);
void ag_delay_free(AgDelay *d);
void ag_delay_set_delay(AgDelay *d, float delay_ms);
void ag_delay_set_feedback(AgDelay *d, float fb);
void ag_delay_set_wet(AgDelay *d, float wet);
void ag_delay_set_dry(AgDelay *d, float dry);
void ag_delay_set_filter(AgDelay *d, float cutoff, float q);
float ag_delay_process(AgDelay *d, float in);
void ag_delay_process_block(AgDelay *d, float *buf, int frames);

/* Ping-pong stereo delay */
typedef struct AgPingPong {
    AgDelay left;
    AgDelay right;
    float cross_feedback;
} AgPingPong;

void ag_pingpong_init(AgPingPong *pp, int sr, float max_delay_ms);
void ag_pingpong_free(AgPingPong *pp);
void ag_pingpong_set_delay(AgPingPong *pp, float delay_ms);
void ag_pingpong_set_feedback(AgPingPong *pp, float fb);
void ag_pingpong_set_cross(AgPingPong *pp, float cross);
void ag_pingpong_process(AgPingPong *pp, float in_l, float in_r, float *out_l, float *out_r);

/* Tape delay with wow/flutter */
typedef struct AgTapeDelay {
    AgDelay delay;
    AgOsc wow_osc;
    AgOsc flutter_osc;
    float wow_depth;
    float flutter_depth;
} AgTapeDelay;

void ag_tape_delay_init(AgTapeDelay *td, int sr, float max_delay_ms);
void ag_tape_delay_free(AgTapeDelay *td);
void ag_tape_delay_set(AgTapeDelay *td, float delay_ms, float fb, float wow, float flutter);
float ag_tape_delay_process(AgTapeDelay *td, float in);

#ifdef __cplusplus
}
#endif

#endif
