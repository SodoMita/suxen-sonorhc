#include "ag_drums.h"
#include <math.h>
#include <string.h>

void ag_drum_params_default(AgDrumParams *p, AgDrumType type) {
    memset(p,0,sizeof(*p));
    p->type = type;
    p->tune = 1.0f;
    p->gain = 0.8f;
    p->seed = 1;
    switch (type) {
        case AG_DRUM_KICK: p->decay=0.5f; p->snap=0.3f; p->noise_mix=0.05f; break;
        case AG_DRUM_SNARE: p->decay=0.3f; p->snap=0.5f; p->noise_mix=0.6f; break;
        case AG_DRUM_HIHAT_CLOSED: p->decay=0.08f; p->noise_mix=1.0f; p->gain=0.5f; break;
        case AG_DRUM_HIHAT_OPEN: p->decay=0.35f; p->noise_mix=1.0f; p->gain=0.5f; break;
        case AG_DRUM_CLAP: p->decay=0.25f; p->noise_mix=0.8f; break;
        case AG_DRUM_TOM_LOW: p->decay=0.4f; p->tune=0.7f; break;
        case AG_DRUM_TOM_MID: p->decay=0.35f; p->tune=1.0f; break;
        case AG_DRUM_TOM_HIGH: p->decay=0.3f; p->tune=1.5f; break;
        case AG_DRUM_RIM: p->decay=0.08f; p->snap=0.8f; break;
        case AG_DRUM_COWBELL: p->decay=0.5f; p->tune=1.0f; break;
        case AG_DRUM_CYMBAL: p->decay=1.2f; p->noise_mix=0.7f; break;
        default: p->decay=0.3f; break;
    }
}

void ag_drum_voice_init(AgDrumVoice *v, const AgDrumParams *p, float sr) {
    memset(v,0,sizeof(*v));
    v->params = *p;
    v->sr = sr>0?sr:AG_SR_DEFAULT;
    v->active = 1;
    v->t = 0.0;
    ag_noise_init(&v->noise, p->seed ? p->seed : 0xD401A5);
    ag_osc_init(&v->osc, AG_OSC_SINE, v->sr);
    ag_osc_init(&v->osc2, AG_OSC_SINE, v->sr);
    ag_biquad_init(&v->filter);

    float base_freq = 50.0f;
    switch (p->type) {
        case AG_DRUM_KICK: base_freq = 60.0f * p->tune; break;
        case AG_DRUM_SNARE: base_freq = 180.0f * p->tune; break;
        case AG_DRUM_TOM_LOW: base_freq = 90.0f * p->tune; break;
        case AG_DRUM_TOM_MID: base_freq = 140.0f * p->tune; break;
        case AG_DRUM_TOM_HIGH: base_freq = 220.0f * p->tune; break;
        case AG_DRUM_RIM: base_freq = 1200.0f * p->tune; break;
        case AG_DRUM_COWBELL: base_freq = 540.0f * p->tune; break;
        default: base_freq = 200.0f * p->tune; break;
    }
    ag_osc_set_freq(&v->osc, base_freq);
    ag_osc_set_freq(&v->osc2, base_freq*1.5f);

    AgADSR adsr_amp = ag_adsr_perc(p->decay);
    if (p->type==AG_DRUM_KICK) { adsr_amp.attack=0.001f; adsr_amp.decay=p->decay; }
    if (p->type==AG_DRUM_HIHAT_CLOSED || p->type==AG_DRUM_HIHAT_OPEN) {
        adsr_amp.attack=0.001f;
    }
    ag_env_init(&v->env_amp, adsr_amp, v->sr);
    ag_env_trigger(&v->env_amp);

    AgADSR adsr_pitch = {0.001f, 0.08f, 0.0f, 0.02f, 0.5f, 2.0f, 1.0f};
    ag_env_init(&v->env_pitch, adsr_pitch, v->sr);
    ag_env_trigger(&v->env_pitch);

    AgADSR adsr_noise = {0.001f, p->decay*0.6f, 0.0f, 0.05f, 1.0f, 1.0f, 1.0f};
    ag_env_init(&v->env_noise, adsr_noise, v->sr);
    ag_env_trigger(&v->env_noise);

    if (p->type==AG_DRUM_HIHAT_CLOSED || p->type==AG_DRUM_HIHAT_OPEN || p->type==AG_DRUM_CYMBAL) {
        ag_biquad_set(&v->filter, AG_FILTER_HP, 4000.0f, 0.7f, 0, (float)v->sr);
    } else if (p->type==AG_DRUM_SNARE) {
        ag_biquad_set(&v->filter, AG_FILTER_BP, 2000.0f, 0.9f, 0, (float)v->sr);
    }
}

int ag_drum_voice_active(const AgDrumVoice *v) { return v->active; }

float ag_drum_voice_next(AgDrumVoice *v) {
    if (!v->active) return 0.0f;
    double dt = 1.0 / v->sr;
    v->t += dt;

    float env_amp = ag_env_next(&v->env_amp);
    float env_pitch = ag_env_next(&v->env_pitch);
    float env_noise = ag_env_next(&v->env_noise);

    if (ag_env_is_idle(&v->env_amp) && env_amp <= 0.0001f) {
        v->active = 0;
        return 0.0f;
    }

    float out = 0.0f;
    float base_freq = (float)v->osc.freq;

    switch (v->params.type) {
        case AG_DRUM_KICK: {
            float pitch_env = 1.0f + env_pitch * 6.0f;
            ag_osc_set_freq(&v->osc, base_freq * pitch_env);
            float sine = ag_osc_next(&v->osc);
            float click = 0.0f;
            if (v->t < 0.005) click = (float)(sin(v->t*3000*AG_PI)*exp(-v->t*800)) * v->params.snap;
            out = sine * 0.9f + click;
            /* soft clip for punch */
            out = tanhf(out * 1.8f) * 0.7f;
        } break;
        case AG_DRUM_SNARE: {
            ag_osc_set_freq(&v->osc, base_freq * (1.0f + env_pitch*0.5f));
            float tone = ag_osc_next(&v->osc) * 0.5f;
            float noise = ag_noise_white(&v->noise);
            noise = ag_biquad_process(&v->filter, noise);
            out = tone * (1.0f - v->params.noise_mix) + noise * v->params.noise_mix;
            out *= env_amp;
            out += ag_noise_white(&v->noise) * v->params.snap * expf(-(float)v->t*80.0f) * 0.5f;
            break;
        }
        case AG_DRUM_HIHAT_CLOSED:
        case AG_DRUM_HIHAT_OPEN: {
            float n1 = ag_noise_white(&v->noise);
            float n2 = ag_noise_white(&v->noise);
            float n = (n1 + n2*0.5f);
            /* 6 square oscs at high freq for metallic */
            float metal = 0.0f;
            for (int i=0;i<6;i++) {
                double f = 3000 + i*800 + (i%2)*1200;
                v->osc.phase += f / v->sr;
                if (v->osc.phase>=1.0) v->osc.phase-=1.0;
                metal += v->osc.phase < 0.5 ? 1.0f : -1.0f;
            }
            metal *= 0.166f;
            n = n*0.6f + metal*0.4f;
            n = ag_biquad_process(&v->filter, n);
            out = n * env_amp;
            break;
        }
        case AG_DRUM_CLAP: {
            /* multiple bursts */
            v->clap_timer += dt;
            if (v->clap_burst < 3 && v->clap_timer > 0.03) {
                v->clap_burst++;
                v->clap_timer = 0.0;
                ag_env_trigger(&v->env_noise);
            }
            float noise = ag_noise_white(&v->noise);
            ag_biquad_set(&v->filter, AG_FILTER_BP, 1200.0f + v->clap_burst*200.0f, 1.2f, 0, (float)v->sr);
            noise = ag_biquad_process(&v->filter, noise);
            out = noise * env_noise * env_amp;
            break;
        }
        case AG_DRUM_TOM_LOW:
        case AG_DRUM_TOM_MID:
        case AG_DRUM_TOM_HIGH: {
            float pitch_env = 1.0f + env_pitch * 1.2f;
            ag_osc_set_freq(&v->osc, base_freq * pitch_env);
            out = ag_osc_next(&v->osc) * env_amp;
            out = tanhf(out*1.2f);
            break;
        }
        case AG_DRUM_RIM: {
            float sine = ag_osc_next(&v->osc);
            float click = expf(-(float)v->t*120.0f) * ag_noise_white(&v->noise) * 0.8f;
            out = sine*0.3f + click;
            out *= env_amp;
            break;
        }
        case AG_DRUM_COWBELL: {
            float s1 = ag_osc_next(&v->osc);
            float s2 = ag_osc_next(&v->osc2);
            out = (s1 + s2*0.6f) * 0.5f * env_amp;
            break;
        }
        case AG_DRUM_CYMBAL: {
            float noise = ag_noise_white(&v->noise);
            noise = ag_biquad_process(&v->filter, noise);
            out = noise * env_amp * 0.8f;
            break;
        }
        default: out = 0.0f; break;
    }

    if (v->params.type != AG_DRUM_SNARE && v->params.type != AG_DRUM_CLAP) {
        out *= env_amp;
    }

    return out * v->params.gain;
}

int ag_drum_render(const AgDrumParams *params, float *out, int max_frames, float sr) {
    if (!params || !out) return 0;
    AgDrumVoice v;
    ag_drum_voice_init(&v, params, sr);
    int i=0;
    for (; i<max_frames; i++) {
        if (!v.active) break;
        out[i] = ag_drum_voice_next(&v);
    }
    for (; i<max_frames; i++) out[i]=0.0f;
    return i;
}
int ag_drum_render_stereo(const AgDrumParams *params, float *stereo_interleaved, int max_frames, float sr) {
    if (!params || !stereo_interleaved) return 0;
    AgDrumVoice v;
    ag_drum_voice_init(&v, params, sr);
    int i=0;
    for (; i<max_frames; i++) {
        if (!v.active) break;
        float s = ag_drum_voice_next(&v);
        stereo_interleaved[i*2]=s;
        stereo_interleaved[i*2+1]=s;
    }
    for (; i<max_frames; i++) {
        stereo_interleaved[i*2]=0;
        stereo_interleaved[i*2+1]=0;
    }
    return i;
}

int ag_drum_kick(float *out, int max_frames, float sr, float tune) {
    AgDrumParams p; ag_drum_params_default(&p, AG_DRUM_KICK); p.tune=tune;
    return ag_drum_render(&p,out,max_frames,sr);
}
int ag_drum_snare(float *out, int max_frames, float sr, float tune) {
    AgDrumParams p; ag_drum_params_default(&p, AG_DRUM_SNARE); p.tune=tune;
    return ag_drum_render(&p,out,max_frames,sr);
}
int ag_drum_hihat(float *out, int max_frames, float sr, int open) {
    AgDrumParams p; ag_drum_params_default(&p, open?AG_DRUM_HIHAT_OPEN:AG_DRUM_HIHAT_CLOSED);
    return ag_drum_render(&p,out,max_frames,sr);
}
int ag_drum_clap(float *out, int max_frames, float sr) {
    AgDrumParams p; ag_drum_params_default(&p, AG_DRUM_CLAP);
    return ag_drum_render(&p,out,max_frames,sr);
}

/* Pattern */
void ag_drum_pattern_init(AgDrumPattern *pat, float bpm) {
    memset(pat,0,sizeof(*pat));
    pat->bpm = bpm>0?bpm:120.0f;
    pat->swing = 0.0f;
    for (int t=0;t<AG_DRUM_TRACKS;t++) {
        ag_drum_params_default(&pat->kits[t], (AgDrumType)(t % AG_DRUM_COUNT));
    }
}
void ag_drum_pattern_set_kit(AgDrumPattern *pat, int track, AgDrumType type) {
    if (track<0||track>=AG_DRUM_TRACKS) return;
    ag_drum_params_default(&pat->kits[track], type);
}
void ag_drum_pattern_set_step(AgDrumPattern *pat, int track, int step, int vel) {
    if (track<0||track>=AG_DRUM_TRACKS) return;
    if (step<0||step>=AG_DRUM_PATTERN_STEPS) return;
    pat->steps[track][step]=vel;
}
void ag_drum_machine_init(AgDrumMachine *dm, const AgDrumPattern *pat, float sr) {
    memset(dm,0,sizeof(*dm));
    dm->pattern = *pat;
    dm->sr = sr>0?sr:AG_SR_DEFAULT;
    dm->playhead = 0.0;
    dm->current_step = 0;
    dm->step_dur = 60.0 / pat->bpm / 4.0; /* 16th notes */
    dm->next_step_time = dm->step_dur;
    ag_rng_seed(&dm->rng, 0xD0101);
    for (int i=0;i<AG_DRUM_TRACKS;i++) {
        dm->voices[i].active=0;
    }
}
void ag_drum_machine_trigger_step(AgDrumMachine *dm, int step) {
    for (int tr=0;tr<AG_DRUM_TRACKS;tr++) {
        int vel = dm->pattern.steps[tr][step % AG_DRUM_PATTERN_STEPS];
        if (vel>0) {
            AgDrumParams p = dm->pattern.kits[tr];
            p.gain *= (vel/100.0f);
            ag_drum_voice_init(&dm->voices[tr], &p, (float)dm->sr);
        }
    }
}
float ag_drum_machine_next(AgDrumMachine *dm) {
    double dt = 1.0/dm->sr;
    dm->playhead += dt;
    if (dm->playhead >= dm->next_step_time) {
        dm->playhead = 0;
        dm->next_step_time = dm->step_dur;
        /* swing on odd steps */
        if (dm->pattern.swing>0 && (dm->current_step%2)==1) {
            dm->next_step_time += dm->step_dur * dm->pattern.swing;
        }
        ag_drum_machine_trigger_step(dm, dm->current_step);
        dm->current_step = (dm->current_step+1)%AG_DRUM_PATTERN_STEPS;
    }
    float mix=0.0f;
    for (int i=0;i<AG_DRUM_TRACKS;i++) {
        if (dm->voices[i].active) mix += ag_drum_voice_next(&dm->voices[i]);
    }
    return ag_soft_clip(mix);
}
void ag_drum_machine_render(AgDrumMachine *dm, float *out, int frames) {
    for (int i=0;i<frames;i++) out[i]=ag_drum_machine_next(dm);
}
