#include "ag_fm.h"
#include <math.h>
#include <string.h>

void ag_fm_op_init(AgFmOp *op, double sr, float freq_mul, float level) {
    memset(op,0,sizeof(*op));
    ag_osc_init(&op->osc, AG_OSC_SINE, sr);
    AgADSR adsr = ag_adsr_pad();
    ag_env_init(&op->env, adsr, sr);
    op->freq_mul = freq_mul>0?freq_mul:1.0f;
    op->level = level;
    op->detune = 0.0f;
}

void ag_fm_voice2_init(AgFmVoice2 *v, double sr, float base_freq, float mod_index) {
    memset(v,0,sizeof(*v));
    v->sr = sr>0?sr:AG_SR_DEFAULT;
    v->mod_index = mod_index;
    v->gain = 0.7f;
    v->feedback = 0.0f;
    ag_fm_op_init(&v->mod, sr, 1.0f, 1.0f);
    ag_fm_op_init(&v->car, sr, 1.0f, 1.0f);
    (void)base_freq;
}
void ag_fm_voice2_note_on(AgFmVoice2 *v, float freq, float vel) {
    if (freq < 20) freq = 20;
    ag_osc_set_freq(&v->mod.osc, freq * v->mod.freq_mul);
    ag_osc_set_freq(&v->car.osc, freq * v->car.freq_mul);
    ag_env_trigger(&v->mod.env);
    ag_env_trigger(&v->car.env);
    v->active = 1;
    v->gain = 0.7f * ag_clamp_f(vel,0,1);
}
void ag_fm_voice2_note_off(AgFmVoice2 *v) {
    ag_env_release(&v->mod.env);
    ag_env_release(&v->car.env);
}
int ag_fm_voice2_active(const AgFmVoice2 *v) {
    return v->active && (!ag_env_is_idle(&v->car.env) || !ag_env_is_idle(&v->mod.env));
}
float ag_fm_voice2_next(AgFmVoice2 *v) {
    if (!ag_fm_voice2_active(v)) { v->active=0; return 0.0f; }
    float env_mod = ag_env_next(&v->mod.env);
    float env_car = ag_env_next(&v->car.env);
    float mod_out = ag_osc_next(&v->mod.osc) * env_mod * v->mod.level;
    /* feedback on modulator */
    if (v->feedback>0.001f) {
        mod_out += v->last_mod * v->feedback;
    }
    v->last_mod = mod_out;
    float mod_phase = mod_out * v->mod_index;
    /* FM: carrier phase modulated */
    double car_inc = v->car.osc.freq / v->car.osc.sr;
    v->car.osc.phase += car_inc + mod_phase * 0.1f;
    if (v->car.osc.phase >= 1.0) v->car.osc.phase -= 1.0;
    if (v->car.osc.phase < 0) v->car.osc.phase += 1.0;
    float car = sinf((float)(v->car.osc.phase * AG_TAU)) * env_car * v->car.level;
    return car * v->gain;
}

/* 4-op */
void ag_fm_voice4_init(AgFmVoice4 *v, double sr, AgFmAlg4 alg) {
    memset(v,0,sizeof(*v));
    v->sr = sr>0?sr:AG_SR_DEFAULT;
    v->alg = alg;
    v->gain = 0.6f;
    for (int i=0;i<4;i++) ag_fm_op_init(&v->ops[i], sr, 1.0f + i*0.5f, 0.8f);
}
void ag_fm_voice4_note_on(AgFmVoice4 *v, float freq, float vel) {
    if (freq<20) freq=20;
    for (int i=0;i<4;i++) {
        ag_osc_set_freq(&v->ops[i].osc, freq * v->ops[i].freq_mul);
        ag_env_trigger(&v->ops[i].env);
    }
    v->active=1;
    v->gain=0.6f*ag_clamp_f(vel,0,1);
}
void ag_fm_voice4_note_off(AgFmVoice4 *v) {
    for (int i=0;i<4;i++) ag_env_release(&v->ops[i].env);
}
int ag_fm_voice4_active(const AgFmVoice4 *v) {
    for (int i=0;i<4;i++) if (!ag_env_is_idle(&v->ops[i].env)) return 1;
    return 0;
}
float ag_fm_voice4_next(AgFmVoice4 *v) {
    if (!ag_fm_voice4_active(v)) { v->active=0; return 0.0f; }
    float env[4];
    float out_op[4];
    for (int i=0;i<4;i++) {
        env[i]=ag_env_next(&v->ops[i].env);
        out_op[i]=ag_osc_next(&v->ops[i].osc) * env[i] * v->ops[i].level;
    }
    float out=0.0f;
    switch (v->alg) {
        case AG_FM_ALG_4_STACK: {
            float m1 = out_op[0];
            float m2 = sinf((float)(v->ops[1].osc.phase * AG_TAU + m1*1.5f)) * env[1];
            float m3 = sinf((float)(v->ops[2].osc.phase * AG_TAU + m2*1.5f)) * env[2];
            out = sinf((float)(v->ops[3].osc.phase * AG_TAU + m3*1.5f)) * env[3];
            /* keep phases moving - we already advanced via osc_next, so override with FM manually for stack */
            /* simplified: just sum modulated */
            out = (m1*0.2f + m2*0.3f + m3*0.3f + out*0.5f);
        } break;
        case AG_FM_ALG_3_PLUS_1: {
            float stack = out_op[0] + out_op[1]*0.5f;
            float m = sinf((float)(v->ops[2].osc.phase * AG_TAU + stack)) * env[2];
            out = m*0.6f + out_op[3]*0.4f;
        } break;
        case AG_FM_ALG_2x2: {
            float a = sinf((float)(v->ops[1].osc.phase * AG_TAU + out_op[0])) * env[1];
            float b = sinf((float)(v->ops[3].osc.phase * AG_TAU + out_op[2])) * env[3];
            out = (a + b)*0.5f;
        } break;
        default: {
            float sum = out_op[0]+out_op[1]+out_op[2];
            out = sinf((float)(v->ops[3].osc.phase * AG_TAU + sum*0.5f)) * env[3];
        } break;
    }
    return out * v->gain;
}

void ag_fm_apply_preset_2op(AgFmVoice2 *v, AgFmPreset preset) {
    switch (preset) {
        case AG_FM_PRESET_BASS:
            v->mod.freq_mul=1.0f; v->car.freq_mul=1.0f;
            v->mod_index=2.5f; v->mod.level=0.9f; v->car.level=0.9f;
            v->mod.env.adsr = (AgADSR){0.01f,0.2f,0.6f,0.15f,1,1,1};
            v->car.env.adsr = (AgADSR){0.01f,0.2f,0.7f,0.2f,1,1,1};
            break;
        case AG_FM_PRESET_LEAD:
            v->mod.freq_mul=1.0f; v->car.freq_mul=1.0f;
            v->mod_index=4.0f; v->mod.level=1.0f; v->car.level=1.0f;
            v->mod.env.adsr = (AgADSR){0.02f,0.3f,0.8f,0.3f,1,1,1};
            v->car.env.adsr = (AgADSR){0.02f,0.3f,0.8f,0.3f,1,1,1};
            break;
        case AG_FM_PRESET_PAD:
            v->mod.freq_mul=0.5f; v->car.freq_mul=1.0f;
            v->mod_index=1.5f;
            v->mod.env.adsr = ag_adsr_pad();
            v->car.env.adsr = ag_adsr_pad();
            break;
        case AG_FM_PRESET_BELL:
            v->mod.freq_mul=3.5f; v->car.freq_mul=1.0f;
            v->mod_index=6.0f;
            v->mod.env.adsr = (AgADSR){0.001f,1.2f,0.0f,0.3f,0.5f,1.5f,1.0f};
            v->car.env.adsr = (AgADSR){0.001f,1.5f,0.0f,0.4f,0.5f,1.5f,1.0f};
            break;
        case AG_FM_PRESET_EPIANO:
            v->mod.freq_mul=1.0f; v->car.freq_mul=1.0f;
            v->mod_index=2.0f;
            v->mod.env.adsr = (AgADSR){0.005f,0.6f,0.2f,0.4f,1,1.2f,1};
            v->car.env.adsr = (AgADSR){0.005f,0.8f,0.3f,0.5f,1,1.2f,1};
            break;
        case AG_FM_PRESET_BRASS:
            v->mod.freq_mul=1.0f; v->car.freq_mul=1.0f;
            v->mod_index=3.0f;
            v->mod.env.adsr = (AgADSR){0.08f,0.2f,0.8f,0.2f,1,1,1};
            v->car.env.adsr = (AgADSR){0.08f,0.2f,0.9f,0.25f,1,1,1};
            break;
        default: break;
    }
}
void ag_fm_apply_preset_4op(AgFmVoice4 *v, AgFmPreset preset) {
    for (int i=0;i<4;i++) {
        v->ops[i].freq_mul = 1.0f + i*0.5f;
        v->ops[i].level = 0.8f;
    }
    switch (preset) {
        case AG_FM_PRESET_BASS: v->alg=AG_FM_ALG_4_STACK; break;
        case AG_FM_PRESET_PAD: v->alg=AG_FM_ALG_2x2; break;
        case AG_FM_PRESET_BELL: v->alg=AG_FM_ALG_4_STACK; v->ops[0].freq_mul=3.5f; break;
        default: v->alg=AG_FM_ALG_2x2; break;
    }
}
