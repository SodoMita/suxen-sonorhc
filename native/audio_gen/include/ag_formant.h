#ifndef AG_FORMANT_H
#define AG_FORMANT_H

#include "ag_common.h"
#include "ag_osc.h"
#include "ag_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Formant synthesis for vowel-like sounds and speech-ish textures */

typedef struct AgFormant {
    float freq;
    float bw;
    float gain;
} AgFormant;

typedef struct AgVowel {
    const char *name;
    AgFormant f[3]; /* first 3 formants */
} AgVowel;

/* Common vowels */
extern const AgVowel AG_VOWEL_A;
extern const AgVowel AG_VOWEL_E;
extern const AgVowel AG_VOWEL_I;
extern const AgVowel AG_VOWEL_O;
extern const AgVowel AG_VOWEL_U;

typedef struct AgFormantFilter {
    AgBiquad filters[3];
    double sr;
} AgFormantFilter;

void ag_formant_filter_init(AgFormantFilter *ff, double sr);
void ag_formant_filter_set_vowel(AgFormantFilter *ff, const AgVowel *vowel);
void ag_formant_filter_set_formants(AgFormantFilter *ff, const AgFormant *formants, int count);
float ag_formant_filter_process(AgFormantFilter *ff, float in);

/* Formant oscillator - source is buzz (saw) + formant filters */
typedef struct AgFormantVoice {
    AgOsc src; /* saw or pulse */
    AgFormantFilter filter;
    float gain;
    double sr;
} AgFormantVoice;

void ag_formant_voice_init(AgFormantVoice *v, double sr);
void ag_formant_voice_set_vowel(AgFormantVoice *v, const AgVowel *vowel);
void ag_formant_voice_set_freq(AgFormantVoice *v, float freq);
float ag_formant_voice_next(AgFormantVoice *v);

/* Morph between two vowels */
typedef struct AgVowelMorph {
    AgVowel a,b;
    float morph; /* 0..1 */
    AgFormantFilter filter;
} AgVowelMorph;

void ag_vowel_morph_init(AgVowelMorph *vm, double sr, const AgVowel *a, const AgVowel *b);
void ag_vowel_morph_set(AgVowelMorph *vm, float morph);
float ag_vowel_morph_process(AgVowelMorph *vm, float in);

#ifdef __cplusplus
}
#endif

#endif
