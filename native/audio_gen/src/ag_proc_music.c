#include "ag_proc_music.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

static double bar_len_from_bpm(float bpm) {
    if (bpm < 10) bpm=10;
    if (bpm > 400) bpm=400;
    return 60.0 / bpm * 4.0;
}
static void set_ramp(float *step, float cur, float tgt, float fade_sec, double sr) {
    float dist = fabsf(tgt - cur);
    if (fade_sec <= 0.0001f) { *step = dist>0?dist:1.0f; return; }
    *step = dist / (fade_sec * (float)sr);
    if (*step < 1e-12f) *step=1e-12f;
}
static void ramp_toward(float *v, float tgt, float step) {
    if (*v < tgt) { *v+=step; if (*v>tgt) *v=tgt; }
    else if (*v > tgt) { *v-=step; if (*v<tgt) *v=tgt; }
}

void ag_proc_spec_default(AgProcSpec *spec, AgMood mood, uint64_t seed) {
    memset(spec,0,sizeof(*spec));
    spec->mood = mood;
    spec->bpm = 90.0f;
    spec->root_midi = 60;
    ag_scale_major(&spec->scale);
    spec->prog_len = 4;
    for (int i=0;i<4;i++) ag_chord_make(&spec->progression[i], AG_CHORD_MAJOR, 0);
    spec->pad_gain=0.4f; spec->pluck_gain=0.25f; spec->bass_gain=0.35f; spec->drums_gain=0.0f;
    spec->plucks_per_bar=4; spec->bass_hits_per_bar=1;
    spec->pluck_shift=12; spec->bass_shift=-12;
    spec->shape=0;
    spec->seed=seed?seed:1;
    spec->id=1;
}

void ag_proc_spec_from_mood(AgProcSpec *spec, AgMood mood, int root_midi, float bpm, uint64_t seed) {
    ag_proc_spec_default(spec, mood, seed);
    spec->mood=mood;
    spec->root_midi=root_midi?root_midi:60;
    spec->bpm=bpm>0?bpm:90.0f;
    switch (mood) {
        case AG_MOOD_CALM:
            ag_scale_pentatonic_major(&spec->scale);
            ag_chord_make(&spec->progression[0], AG_CHORD_MAJOR,0);
            ag_chord_make(&spec->progression[1], AG_CHORD_MAJOR,1);
            ag_chord_make(&spec->progression[2], AG_CHORD_SUS2,0);
            ag_chord_make(&spec->progression[3], AG_CHORD_MAJOR,0);
            spec->prog_len=4; spec->pad_gain=0.52f; spec->pluck_gain=0.22f; spec->bass_gain=0.3f;
            spec->plucks_per_bar=3; spec->shape=0; break;
        case AG_MOOD_WARM:
            ag_scale_major(&spec->scale);
            ag_chord_make(&spec->progression[0], AG_CHORD_MAJOR,0);
            ag_chord_make(&spec->progression[1], AG_CHORD_MAJ7,0);
            ag_chord_make(&spec->progression[2], AG_CHORD_MINOR,0);
            ag_chord_make(&spec->progression[3], AG_CHORD_SUS4,0);
            spec->prog_len=4; spec->pad_gain=0.54f; spec->pluck_gain=0.26f; spec->bass_gain=0.32f;
            spec->plucks_per_bar=4; break;
        case AG_MOOD_TENSE:
            ag_scale_minor(&spec->scale);
            ag_chord_make(&spec->progression[0], AG_CHORD_MINOR,0);
            ag_chord_make(&spec->progression[1], AG_CHORD_DIM,0);
            ag_chord_make(&spec->progression[2], AG_CHORD_MINOR,1);
            ag_chord_make(&spec->progression[3], AG_CHORD_DOM7,0);
            spec->prog_len=4; spec->pad_gain=0.35f; spec->pluck_gain=0.32f; spec->bass_gain=0.5f;
            spec->plucks_per_bar=8; spec->bass_hits_per_bar=2; spec->shape=2; spec->bpm=96; break;
        case AG_MOOD_NIGHT:
            ag_scale_pentatonic_minor(&spec->scale);
            ag_chord_make(&spec->progression[0], AG_CHORD_MINOR,0);
            ag_chord_make(&spec->progression[1], AG_CHORD_SUS2,0);
            ag_chord_make(&spec->progression[2], AG_CHORD_MAJOR,0);
            ag_chord_make(&spec->progression[3], AG_CHORD_MINOR,0);
            spec->prog_len=4; spec->pad_gain=0.5f; spec->pluck_gain=0.18f; spec->bass_gain=0.28f;
            spec->plucks_per_bar=2; spec->bpm=60; break;
        case AG_MOOD_DREAM:
            ag_scale_lydian(&spec->scale);
            spec->pad_gain=0.6f; spec->pluck_gain=0.15f; spec->bass_gain=0.2f;
            spec->plucks_per_bar=2; spec->bpm=56; spec->shape=1; break;
        case AG_MOOD_LOFI:
            ag_scale_dorian(&spec->scale);
            ag_chord_make(&spec->progression[0], AG_CHORD_MIN7,0);
            ag_chord_make(&spec->progression[1], AG_CHORD_DOM7,0);
            ag_chord_make(&spec->progression[2], AG_CHORD_MAJ7,0);
            ag_chord_make(&spec->progression[3], AG_CHORD_MIN7,1);
            spec->prog_len=4; spec->pad_gain=0.45f; spec->pluck_gain=0.2f; spec->bass_gain=0.4f;
            spec->drums_gain=0.5f; spec->plucks_per_bar=4; spec->bpm=82; break;
        case AG_MOOD_CHIPTUNE:
            ag_scale_major(&spec->scale);
            spec->pad_gain=0.2f; spec->pluck_gain=0.5f; spec->bass_gain=0.3f;
            spec->plucks_per_bar=8; spec->bpm=120; spec->shape=1; break;
        case AG_MOOD_AMBIENT:
            ag_scale_whole_tone(&spec->scale);
            spec->pad_gain=0.65f; spec->pluck_gain=0.1f; spec->bass_gain=0.15f;
            spec->plucks_per_bar=1; spec->bpm=48; spec->shape=0; break;
        case AG_MOOD_RIFT:
            ag_scale_octatonic(&spec->scale);
            spec->pad_gain=0.2f; spec->pluck_gain=0.35f; spec->bass_gain=0.6f;
            spec->plucks_per_bar=8; spec->bass_hits_per_bar=2; spec->shape=2; spec->bpm=128; break;
        case AG_MOOD_FESTIVAL:
            ag_scale_mixolydian(&spec->scale);
            spec->pad_gain=0.3f; spec->pluck_gain=0.45f; spec->bass_gain=0.35f;
            spec->plucks_per_bar=6; spec->bass_hits_per_bar=2; spec->shape=1; spec->bpm=114; break;
        case AG_MOOD_LAB:
            ag_scale_minor(&spec->scale);
            spec->pad_gain=0.18f; spec->pluck_gain=0.38f; spec->bass_gain=0.45f;
            spec->plucks_per_bar=8; spec->bass_hits_per_bar=2; spec->shape=2; spec->bpm=108; break;
        default: break;
    }
}

static void queue_add(AgProcMusic *pm, AgProcEvent ev) {
    if (pm->queue_n >= AG_PROC_QUEUE) return;
    int i=pm->queue_n;
    while (i>0 && pm->queue[i-1].time > ev.time) { pm->queue[i]=pm->queue[i-1]; i--; }
    pm->queue[i]=ev; pm->queue_n++;
}

static void schedule_bar(AgProcMusic *pm) {
    double bar_len = pm->bar_len>0?pm->bar_len:bar_len_from_bpm(pm->spec.bpm);
    double bt = pm->next_bar;
    int bar = pm->bar_index;
    if (pm->spec.prog_len<=0) { pm->next_bar=bt+bar_len; pm->bar_index++; return; }
    int chord_i = bar % pm->spec.prog_len;
    if (chord_i<0) chord_i+=pm->spec.prog_len;
    AgChord *ch = &pm->spec.progression[chord_i];
    /* pad */
    for (int i=0;i<ch->count;i++) {
        AgProcEvent ev; ev.time=bt;
        ev.midi = pm->spec.root_midi + ch->tones[i] + 12; /* up octave */
        ev.kind=0; ev.dur=(float)(bar_len*1.15); ev.vel=pm->spec.pad_gain / (ch->count>0?ch->count:1);
        ev.pan = (i%2==1)?0.22f:-0.22f;
        queue_add(pm,ev); pm->notes_scheduled++;
    }
    /* bass */
    for (int i=0;i<pm->spec.bass_hits_per_bar;i++) {
        AgProcEvent ev;
        ev.time=bt + bar_len*0.5*i;
        int root_deg = ch->count>0?ch->tones[0]:0;
        ev.midi = pm->spec.root_midi + root_deg + pm->spec.bass_shift;
        ev.kind=2; ev.dur=(float)(bar_len*0.45); ev.vel=pm->spec.bass_gain; ev.pan=0.0f;
        queue_add(pm,ev); pm->notes_scheduled++;
    }
    /* plucks */
    for (int i=0;i<pm->spec.plucks_per_bar;i++) {
        AgProcEvent ev;
        ev.time=bt + bar_len * ((double)i / (double)pm->spec.plucks_per_bar);
        int deg = ch->count>0? ch->tones[(i+bar)%ch->count] : 0;
        ev.midi = pm->spec.root_midi + deg + pm->spec.pluck_shift;
        if (pm->spec.plucks_per_bar>=8 && i%3==2) ev.midi+=12;
        ev.kind=1; ev.dur=(float)(bar_len*0.6); ev.vel=pm->spec.pluck_gain;
        ev.pan = ag_rng_range_f32(&pm->rng, -0.35f, 0.35f);
        queue_add(pm,ev); pm->notes_scheduled++;
    }
    pm->next_bar = bt + bar_len;
    pm->bar_index++;
}

static void voice_add(AgProcMusic *pm, AgProcEvent *ev) {
    if (pm->voice_n >= AG_PROC_MAX_VOICES) return;
    AgProcVoice *v = &pm->voices[pm->voice_n++];
    memset(v,0,sizeof(*v));
    double freq = ag_midi_to_freq(ev->midi);
    ag_osc_init(&v->osc, AG_OSC_SINE, pm->sr);
    ag_osc_set_freq(&v->osc, freq);
    ag_osc_init(&v->osc2, AG_OSC_SINE, pm->sr);
    ag_osc_set_freq(&v->osc2, freq*1.01);
    AgADSR adsr;
    if (ev->kind==1) adsr = ag_adsr_pluck();
    else if (ev->kind==2) adsr = ag_adsr_bass();
    else adsr = ag_adsr_pad();
    ag_env_init(&v->env, adsr, pm->sr);
    ag_env_trigger(&v->env);
    v->dur = ev->dur;
    v->peak = ev->vel;
    v->pan = ev->pan;
    v->kind = ev->kind;
    v->active=1;
    v->gl = 1.0f - 0.6f * (ev->pan>0?ev->pan:0);
    v->gr = 1.0f - 0.6f * (ev->pan<0?-ev->pan:0);
    ag_biquad_init(&v->filter);
    if (ev->kind==2) ag_biquad_set(&v->filter, AG_FILTER_LP, 250.0f, 0.7f, 0, (float)pm->sr);
}

void ag_proc_init(AgProcMusic *pm, const AgProcSpec *spec, double sr) {
    memset(pm,0,sizeof(*pm));
    pm->sr = sr>0?sr:AG_SR_DEFAULT;
    pm->spec = *spec;
    ag_rng_seed(&pm->rng, spec->seed?spec->seed:1);
    pm->bar_len = bar_len_from_bpm(spec->bpm);
    pm->gain = 0.0f; pm->gain_target=1.0f;
    set_ramp(&pm->gain_step, 0,1,0.8f, pm->sr);
    pm->notes_scheduled=0;
    ag_reverb_init(&pm->reverb, (int)pm->sr);
    ag_reverb_set_room_size(&pm->reverb, 0.6f);
    ag_reverb_set_wet(&pm->reverb, 0.25f);
    ag_reverb_set_dry(&pm->reverb, 0.8f);
    ag_delay_init(&pm->delay, (int)pm->sr, 600.0f);
    ag_delay_set_delay(&pm->delay, 340.0f);
    ag_delay_set_feedback(&pm->delay, 0.25f);
    ag_delay_set_wet(&pm->delay, 0.15f);
    pm->use_reverb = (spec->mood==AG_MOOD_AMBIENT || spec->mood==AG_MOOD_DREAM || spec->mood==AG_MOOD_NIGHT);
    pm->use_delay = (spec->mood==AG_MOOD_TENSE || spec->mood==AG_MOOD_RIFT || spec->mood==AG_MOOD_CHIPTUNE);
    if (spec->drums_gain>0.01f) {
        AgDrumPattern pat; ag_drum_pattern_init(&pat, spec->bpm);
        ag_drum_pattern_set_kit(&pat,0,AG_DRUM_KICK);
        ag_drum_pattern_set_kit(&pat,1,AG_DRUM_SNARE);
        ag_drum_pattern_set_kit(&pat,2,AG_DRUM_HIHAT_CLOSED);
        ag_drum_pattern_set_step(&pat,0,0,100); ag_drum_pattern_set_step(&pat,0,4,100);
        ag_drum_pattern_set_step(&pat,0,8,100); ag_drum_pattern_set_step(&pat,0,12,100);
        ag_drum_pattern_set_step(&pat,1,4,80); ag_drum_pattern_set_step(&pat,1,12,80);
        for(int i=0;i<16;i+=2) ag_drum_pattern_set_step(&pat,2,i,60);
        ag_drum_machine_init(&pm->drums, &pat, (float)pm->sr);
        pm->has_drums=1;
    }
}
void ag_proc_set_spec(AgProcMusic *pm, const AgProcSpec *spec, float fade_sec) {
    uint64_t id = pm->spec.id;
    uint64_t seed = pm->spec.seed;
    double next_bar = pm->next_bar;
    double playhead = pm->playhead;
    double bar_len_old = pm->bar_len;
    pm->spec = *spec;
    pm->spec.id = id;
    pm->spec.seed = seed;
    double new_len = bar_len_from_bpm(spec->bpm);
    if (next_bar > playhead && bar_len_old>0) {
        double start = next_bar - bar_len_old;
        if (start < playhead) start = playhead;
        pm->next_bar = start + new_len;
    }
    pm->bar_len = new_len;
    (void)fade_sec;
}
void ag_proc_set_gain(AgProcMusic *pm, float gain, float fade_sec) {
    pm->gain_target = ag_clamp_f(gain,0,1.5f);
    set_ramp(&pm->gain_step, pm->gain, pm->gain_target, fade_sec, pm->sr);
}
int ag_proc_active(const AgProcMusic *pm) {
    if (pm->gain>0.0005f) return 1;
    for (int i=0;i<pm->voice_n;i++) if (pm->voices[i].active) return 1;
    return 0;
}
static void spawn_due(AgProcMusic *pm, double block_end) {
    while (pm->queue_n>0 && pm->queue[0].time <= block_end) {
        AgProcEvent ev = pm->queue[0];
        memmove(&pm->queue[0], &pm->queue[1], (pm->queue_n-1)*sizeof(AgProcEvent));
        pm->queue_n--;
        voice_add(pm,&ev);
    }
}
void ag_proc_render(AgProcMusic *pm, float *interleaved_stereo, int frames) {
    double inv_sr = 1.0/pm->sr;
    int pos=0;
    while (pos<frames) {
        int block=256;
        if (block>frames-pos) block=frames-pos;
        double block_end = pm->playhead + block*inv_sr;
        if (pm->next_bar <= block_end + 1e-9) {
            int guard=0;
            while (pm->next_bar <= block_end + 1e-9 && guard<32) { schedule_bar(pm); guard++; }
        }
        spawn_due(pm, block_end);
        for (int i=0;i<block;i++) {
            double l=0,r=0;
            int vi=0;
            while (vi<pm->voice_n) {
                AgProcVoice *v=&pm->voices[vi];
                float env = ag_env_next(&v->env);
                if (env<=0.0002f && ag_env_is_idle(&v->env)) {
                    pm->voice_n--;
                    if (vi!=pm->voice_n) pm->voices[vi]=pm->voices[pm->voice_n];
                    continue;
                }
                float raw = ag_osc_next(&v->osc);
                if (pm->spec.shape>=2) {
                    float s = raw>0?1.0f:-1.0f;
                    raw = s*0.72f + raw*0.28f;
                } else if (pm->spec.shape>=1) {
                    raw = asinf(ag_clamp_f(raw,-1,1))*0.63662f;
                }
                float samp = raw * env * v->peak;
                l += samp * v->gl;
                r += samp * v->gr;
                vi++;
            }
            if (pm->has_drums) {
                float d = ag_drum_machine_next(&pm->drums) * pm->spec.drums_gain;
                l+=d*0.5f; r+=d*0.5f;
            }
            l = l / (1.0 + fabs(l)) * 1.4;
            r = r / (1.0 + fabs(r)) * 1.4;
            ramp_toward(&pm->gain, pm->gain_target, pm->gain_step);
            l*=pm->gain; r*=pm->gain;
            if (pm->use_reverb) {
                float rl,rr;
                ag_reverb_process_stereo(&pm->reverb, (float)l, (float)r, &rl, &rr);
                l=rl; r=rr;
            }
            if (pm->use_delay) {
                l = ag_delay_process(&pm->delay, (float)l);
                /* reuse same delay for r with slight offset */
                r = l*0.9f + r*0.1f;
            }
            interleaved_stereo[(pos+i)*2]=(float)l;
            interleaved_stereo[(pos+i)*2+1]=(float)r;
            pm->playhead+=inv_sr;
        }
        pos+=block;
    }
}

/* Mixer */
void ag_proc_mixer_init(AgProcMixer *mix, double sr) {
    memset(mix,0,sizeof(*mix));
    mix->sr=sr>0?sr:AG_SR_DEFAULT;
    mix->primary=-1;
    mix->master=0.0f; mix->master_target=0.5f;
    set_ramp(&mix->master_step, 0,0.5f,0.35f,sr);
}
void ag_proc_mixer_transition(AgProcMixer *mix, const AgProcSpec *spec, float fade_sec) {
    if (!spec) return;
    if (fade_sec<0) fade_sec=0.8f;
    if (mix->primary>=0) {
        AgProcMusic *cur=&mix->layers[mix->primary];
        if (cur->spec.id==spec->id) return;
    }
    int slot=0;
    if (mix->primary>=0) {
        AgProcMusic *out=&mix->layers[mix->primary];
        slot=1-mix->primary;
        out->gain_target=0.0f;
        set_ramp(&out->gain_step, out->gain,0.0f,fade_sec,mix->sr);
    }
    ag_proc_init(&mix->layers[slot], spec, mix->sr);
    mix->primary=slot;
    mix->master_target=0.5f;
    set_ramp(&mix->master_step, mix->master,0.5f, mix->master<0.001f?0.35f:fade_sec, mix->sr);
}
void ag_proc_mixer_adjust(AgProcMixer *mix, const AgProcSpec *spec) {
    if (!spec || mix->primary<0) { ag_proc_mixer_transition(mix,spec,0.35f); return; }
    AgProcMusic *layer=&mix->layers[mix->primary];
    ag_proc_set_spec(layer,spec,0.35f);
}
void ag_proc_mixer_reseed(AgProcMixer *mix, uint64_t seed) {
    if (mix->primary<0) return;
    if (seed==0) seed=1;
    mix->layers[mix->primary].rng.state=seed;
    mix->layers[mix->primary].spec.seed=seed;
}
void ag_proc_mixer_set_gain(AgProcMixer *mix, float gain, float fade_sec) {
    mix->master_target=ag_clamp_f(gain,0,1.5f);
    set_ramp(&mix->master_step, mix->master, mix->master_target, fade_sec, mix->sr);
}
void ag_proc_mixer_render(AgProcMixer *mix, float *interleaved, int frames) {
    float *tmp = (float*)malloc(sizeof(float)*frames*2);
    if (!tmp) return;
    memset(interleaved,0,sizeof(float)*frames*2);
    for (int li=0;li<AG_PROC_LAYERS;li++) {
        AgProcMusic *layer=&mix->layers[li];
        if (layer->gain<=0.0001f && layer->voice_n==0 && layer->queue_n==0 && li!=mix->primary) continue;
        ag_proc_render(layer, tmp, frames);
        for (int i=0;i<frames*2;i++) interleaved[i]+=tmp[i];
    }
    free(tmp);
    for (int i=0;i<frames;i++) {
        ramp_toward(&mix->master, mix->master_target, mix->master_step);
        interleaved[i*2]*=mix->master;
        interleaved[i*2+1]*=mix->master;
    }
}
int ag_proc_mixer_active(const AgProcMixer *mix) {
    if (mix->master>0.0005f) return 1;
    for (int i=0;i<AG_PROC_LAYERS;i++) if (ag_proc_active(&mix->layers[i])) return 1;
    return 0;
}

AgMood ag_mood_from_string(const char *str) {
    if (!str) return AG_MOOD_CALM;
    if (strcmp(str,"calm")==0) return AG_MOOD_CALM;
    if (strcmp(str,"warm")==0) return AG_MOOD_WARM;
    if (strcmp(str,"tense")==0) return AG_MOOD_TENSE;
    if (strcmp(str,"night")==0) return AG_MOOD_NIGHT;
    if (strcmp(str,"dream")==0) return AG_MOOD_DREAM;
    if (strcmp(str,"lofi")==0) return AG_MOOD_LOFI;
    if (strcmp(str,"chiptune")==0) return AG_MOOD_CHIPTUNE;
    if (strcmp(str,"ambient")==0) return AG_MOOD_AMBIENT;
    if (strcmp(str,"rift")==0) return AG_MOOD_RIFT;
    if (strcmp(str,"festival")==0) return AG_MOOD_FESTIVAL;
    if (strcmp(str,"lab")==0) return AG_MOOD_LAB;
    return AG_MOOD_CALM;
}
const char* ag_mood_to_string(AgMood mood) {
    switch (mood) {
        case AG_MOOD_CALM: return "calm";
        case AG_MOOD_WARM: return "warm";
        case AG_MOOD_TENSE: return "tense";
        case AG_MOOD_NIGHT: return "night";
        case AG_MOOD_DREAM: return "dream";
        case AG_MOOD_LOFI: return "lofi";
        case AG_MOOD_CHIPTUNE: return "chiptune";
        case AG_MOOD_AMBIENT: return "ambient";
        case AG_MOOD_RIFT: return "rift";
        case AG_MOOD_FESTIVAL: return "festival";
        case AG_MOOD_LAB: return "lab";
        default: return "calm";
    }
}
