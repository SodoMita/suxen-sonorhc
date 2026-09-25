#include "ag_formant.h"
#include <string.h>
#include <math.h>

const AgVowel AG_VOWEL_A = {"A", {{800,80,1.0f},{1150,90,0.8f},{2900,120,0.6f}}};
const AgVowel AG_VOWEL_E = {"E", {{400,70,1.0f},{1700,80,0.8f},{2600,100,0.6f}}};
const AgVowel AG_VOWEL_I = {"I", {{300,60,1.0f},{2300,90,0.8f},{3000,120,0.6f}}};
const AgVowel AG_VOWEL_O = {"O", {{500,70,1.0f},{800,80,0.8f},{2600,100,0.6f}}};
const AgVowel AG_VOWEL_U = {"U", {{300,60,1.0f},{800,80,0.8f},{2300,100,0.6f}}};

void ag_formant_filter_init(AgFormantFilter *ff, double sr) {
    memset(ff,0,sizeof(*ff));
    ff->sr=sr>0?sr:AG_SR_DEFAULT;
    for(int i=0;i<3;i++) ag_biquad_init(&ff->filters[i]);
}
void ag_formant_filter_set_formants(AgFormantFilter *ff, const AgFormant *formants, int count) {
    if(count>3) count=3;
    for(int i=0;i<count;i++){
        float q = formants[i].freq / formants[i].bw;
        if(q<0.5f) q=0.5f;
        ag_biquad_set(&ff->filters[i], AG_FILTER_BP, formants[i].freq, q, 0, (float)ff->sr);
    }
}
void ag_formant_filter_set_vowel(AgFormantFilter *ff, const AgVowel *vowel) {
    ag_formant_filter_set_formants(ff, vowel->f, 3);
}
float ag_formant_filter_process(AgFormantFilter *ff, float in) {
    float out=0;
    for(int i=0;i<3;i++) out += ag_biquad_process(&ff->filters[i], in);
    return out * 0.33f;
}

void ag_formant_voice_init(AgFormantVoice *v, double sr) {
    memset(v,0,sizeof(*v));
    v->sr=sr>0?sr:AG_SR_DEFAULT;
    v->gain=0.5f;
    ag_osc_init(&v->src, AG_OSC_SAW, sr);
    ag_osc_set_freq(&v->src, 110);
    ag_formant_filter_init(&v->filter, sr);
    ag_formant_filter_set_vowel(&v->filter, &AG_VOWEL_A);
}
void ag_formant_voice_set_vowel(AgFormantVoice *v, const AgVowel *vowel) {
    ag_formant_filter_set_vowel(&v->filter, vowel);
}
void ag_formant_voice_set_freq(AgFormantVoice *v, float freq) {
    ag_osc_set_freq(&v->src, freq);
}
float ag_formant_voice_next(AgFormantVoice *v) {
    float src = ag_osc_next(&v->src);
    float out = ag_formant_filter_process(&v->filter, src);
    return out * v->gain;
}

void ag_vowel_morph_init(AgVowelMorph *vm, double sr, const AgVowel *a, const AgVowel *b) {
    memset(vm,0,sizeof(*vm));
    vm->a=*a; vm->b=*b;
    vm->morph=0;
    ag_formant_filter_init(&vm->filter, sr);
    ag_formant_filter_set_vowel(&vm->filter, a);
}
void ag_vowel_morph_set(AgVowelMorph *vm, float morph) {
    vm->morph=ag_clamp_f(morph,0,1);
    AgFormant f[3];
    for(int i=0;i<3;i++){
        f[i].freq = vm->a.f[i].freq * (1.0f - vm->morph) + vm->b.f[i].freq * vm->morph;
        f[i].bw = vm->a.f[i].bw * (1.0f - vm->morph) + vm->b.f[i].bw * vm->morph;
        f[i].gain = vm->a.f[i].gain * (1.0f - vm->morph) + vm->b.f[i].gain * vm->morph;
    }
    ag_formant_filter_set_formants(&vm->filter, f, 3);
}
float ag_vowel_morph_process(AgVowelMorph *vm, float in) {
    return ag_formant_filter_process(&vm->filter, in);
}
