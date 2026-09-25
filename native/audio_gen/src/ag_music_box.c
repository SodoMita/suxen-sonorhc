#include "ag_music_box.h"
#include <math.h>
#include <string.h>

void ag_ks_init(AgKarplus *ks, double sr) {
    memset(ks,0,sizeof(*ks));
    ks->sr = sr>0?sr:AG_SR_DEFAULT;
    ks->feedback = 0.995f;
    ks->damping = 0.5f;
    ks->gain = 0.7f;
    ag_biquad_init(&ks->filter);
    ag_biquad_set(&ks->filter, AG_FILTER_LP, 4000.0f, 0.7f, 0, (float)sr);
}
void ag_ks_pluck(AgKarplus *ks, float freq, float damping, float gain) {
    if (freq < 20) freq=20;
    int len = (int)(ks->sr / freq);
    if (len < 4) len=4;
    if (len > AG_KS_MAX_DELAY) len=AG_KS_MAX_DELAY;
    ks->buf_len = len;
    ks->pos = 0;
    ks->damping = ag_clamp_f(damping,0,0.99f);
    ks->gain = gain;
    AgRng rng; ag_rng_seed(&rng, (uint64_t)(freq*1000) ^ 0x1234);
    for (int i=0;i<len;i++) ks->buf[i] = ag_rng_range_f32(&rng, -1.0f, 1.0f);
    ks->active=1;
}
float ag_ks_next(AgKarplus *ks) {
    if (!ks->active) return 0.0f;
    float cur = ks->buf[ks->pos];
    int next = (ks->pos+1)%ks->buf_len;
    float nxt = ks->buf[next];
    float avg = (cur + nxt) * 0.5f;
    avg = ag_biquad_process(&ks->filter, avg);
    avg *= ks->feedback;
    /* damping = lowpass in loop */
    avg = avg * (1.0f - ks->damping) + cur * ks->damping * 0.0f;
    ks->buf[ks->pos] = avg;
    ks->pos = next;
    if (fabsf(avg) < 0.0001f && ks->buf_len>0) {
        /* check if decayed */
        float sum=0; for(int i=0;i<ks->buf_len;i++) sum+=fabsf(ks->buf[i]);
        if (sum < 0.001f) ks->active=0;
    }
    return cur * ks->gain;
}
int ag_ks_active(const AgKarplus *ks) { return ks->active; }

void ag_tine_init(AgTine *t, double sr) {
    memset(t,0,sizeof(*t));
    t->sr = sr>0?sr:AG_SR_DEFAULT;
    t->gain=0.6f;
    ag_osc_init(&t->osc, AG_OSC_SINE, sr);
    ag_osc_init(&t->overtone, AG_OSC_SINE, sr);
    AgADSR adsr = {0.001f, 0.8f, 0.0f, 0.2f, 0.3f, 2.0f, 1.0f};
    ag_env_init(&t->env, adsr, sr);
    ag_biquad_init(&t->filter);
    ag_biquad_set(&t->filter, AG_FILTER_LP, 4000.0f, 0.7f, 0, (float)sr);
}
void ag_tine_hit(AgTine *t, float freq, float vel) {
    if (freq<20) freq=20;
    ag_osc_set_freq(&t->osc, freq);
    ag_osc_set_freq(&t->overtone, freq*4.0f);
    t->osc.phase=0; t->overtone.phase=0;
    ag_env_trigger(&t->env);
    t->active=1;
    t->gain=0.6f*ag_clamp_f(vel,0,1);
    ag_biquad_set(&t->filter, AG_FILTER_LP, freq*6.0f, 0.7f, 0, (float)t->sr);
}
float ag_tine_next(AgTine *t) {
    if (!t->active) return 0.0f;
    float env = ag_env_next(&t->env);
    if (ag_env_is_idle(&t->env)) { t->active=0; return 0.0f; }
    float s1 = ag_osc_next(&t->osc);
    float s2 = ag_osc_next(&t->overtone) * 0.3f;
    float out = (s1 + s2) * env;
    out = ag_biquad_process(&t->filter, out);
    return out * t->gain;
}
int ag_tine_active(const AgTine *t) { return t->active; }

void ag_bell_init(AgBell *b, double sr) {
    memset(b,0,sizeof(*b));
    b->sr = sr>0?sr:AG_SR_DEFAULT;
    b->gain=0.6f;
    for (int i=0;i<AG_BELL_PARTIALS;i++) {
        ag_osc_init(&b->partials[i], AG_OSC_SINE, sr);
        b->amps[i]=0.0f;
        b->decays[i]=1.0f;
        b->envs[i]=0.0f;
    }
}
void ag_bell_hit(AgBell *b, float freq, float vel) {
    if (freq<20) freq=20;
    float ratios[AG_BELL_PARTIALS] = {1.0f, 2.0f, 2.4f, 3.0f, 4.2f, 5.4f, 6.8f, 8.0f};
    float amps[AG_BELL_PARTIALS] = {1.0f,0.6f,0.4f,0.3f,0.2f,0.15f,0.1f,0.05f};
    float decays[AG_BELL_PARTIALS] = {1.0f,0.8f,0.7f,0.6f,0.5f,0.4f,0.3f,0.2f};
    for (int i=0;i<AG_BELL_PARTIALS;i++) {
        ag_osc_set_freq(&b->partials[i], freq*ratios[i]);
        b->partials[i].phase=0;
        b->amps[i]=amps[i];
        b->decays[i]=decays[i];
        b->envs[i]=1.0f;
    }
    b->active=1;
    b->gain=0.6f*ag_clamp_f(vel,0,1);
    b->t=0.0;
}
float ag_bell_next(AgBell *b) {
    if (!b->active) return 0.0f;
    b->t += 1.0/b->sr;
    float out=0.0f;
    int alive=0;
    for (int i=0;i<AG_BELL_PARTIALS;i++) {
        if (b->envs[i]<=0.0001f) continue;
        alive=1;
        b->envs[i] *= (1.0f - 0.001f / b->decays[i]);
        if (b->envs[i]<0) b->envs[i]=0;
        float s = ag_osc_next(&b->partials[i]) * b->amps[i] * b->envs[i];
        out += s;
    }
    if (!alive) b->active=0;
    return out * b->gain * 0.4f;
}
int ag_bell_active(const AgBell *b) { return b->active; }

void ag_kalimba_init(AgKalimba *k, double sr) {
    memset(k,0,sizeof(*k));
    ag_tine_init(&k->tine, sr);
    ag_ks_init(&k->wood, sr);
    k->mix=0.7f;
}
void ag_kalimba_hit(AgKalimba *k, float freq, float vel) {
    ag_tine_hit(&k->tine, freq, vel);
    ag_ks_pluck(&k->wood, freq*0.5f, 0.3f, vel*0.3f);
}
float ag_kalimba_next(AgKalimba *k) {
    float t = ag_tine_next(&k->tine);
    float w = ag_ks_next(&k->wood);
    return t*k->mix + w*(1.0f-k->mix);
}

void ag_musicbox_init(AgMusicBox *mb, double sr) {
    memset(mb,0,sizeof(*mb));
    mb->sr = sr>0?sr:AG_SR_DEFAULT;
    mb->gain=0.6f;
    for (int i=0;i<AG_MUSICBOX_VOICES;i++) ag_tine_init(&mb->voices[i], sr);
}
void ag_musicbox_add_note(AgMusicBox *mb, float time, int midi, float vel) {
    if (mb->note_count >= AG_MUSICBOX_NOTES) return;
    mb->notes[mb->note_count].time=time;
    mb->notes[mb->note_count].midi=midi;
    mb->notes[mb->note_count].vel=vel;
    mb->note_count++;
}
void ag_musicbox_clear(AgMusicBox *mb) { mb->note_count=0; mb->playhead=0; mb->next_note=0; }
void ag_musicbox_set_loop(AgMusicBox *mb, int loop) { mb->looping=loop; }
float ag_musicbox_next(AgMusicBox *mb) {
    mb->playhead += 1.0/mb->sr;
    while (mb->next_note < mb->note_count && mb->notes[mb->next_note].time <= mb->playhead) {
        /* find free voice */
        int v=-1;
        for (int i=0;i<AG_MUSICBOX_VOICES;i++) if (!mb->voices[i].active) { v=i; break; }
        if (v==-1) v=0; /* steal */
        float freq = (float)ag_midi_to_freq(mb->notes[mb->next_note].midi);
        ag_tine_hit(&mb->voices[v], freq, mb->notes[mb->next_note].vel);
        mb->next_note++;
    }
    if (mb->next_note >= mb->note_count && mb->looping) {
        mb->playhead=0; mb->next_note=0;
    }
    float mix=0.0f;
    for (int i=0;i<AG_MUSICBOX_VOICES;i++) mix += ag_tine_next(&mb->voices[i]);
    return mix * mb->gain;
}
void ag_musicbox_render(AgMusicBox *mb, float *out, int frames) {
    for (int i=0;i<frames;i++) out[i]=ag_musicbox_next(mb);
}
