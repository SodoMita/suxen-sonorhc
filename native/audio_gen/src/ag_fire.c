#include "ag_fire.h"
#include <string.h>
#include <math.h>

void ag_fire_init(AgFire *f, double sr) {
    memset(f,0,sizeof(*f));
    f->sr=sr>0?sr:AG_SR_DEFAULT;
    f->gain=0.5f;
    f->crackle_density=0.15f;
    f->base_intensity=0.6f;
    ag_noise_init(&f->noise, 0xF111E);
    ag_rng_seed(&f->rng, 0xF111E);
    ag_biquad_init(&f->lp); ag_biquad_set(&f->lp, AG_FILTER_LP, 2500.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&f->bp); ag_biquad_set(&f->bp, AG_FILTER_BP, 800.0f, 0.9f, 0, (float)sr);
    ag_biquad_init(&f->hp); ag_biquad_set(&f->hp, AG_FILTER_HP, 40.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&f->flicker_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&f->flicker_lfo, 8.0f);
}
void ag_fire_set(AgFire *f, float intensity, float crackle_density) {
    f->base_intensity=ag_clamp_f(intensity,0,1);
    f->crackle_density=ag_clamp_f(crackle_density,0,1);
}
float ag_fire_next(AgFire *f) {
    float base = ag_noise_pink(&f->noise) * 0.5f + ag_noise_white(&f->noise)*0.2f;
    base = ag_biquad_process(&f->lp, base);
    base = ag_biquad_process(&f->hp, base);
    float flicker = ag_osc_next(&f->flicker_lfo) * 0.15f;
    base *= (0.8f + flicker + f->base_intensity*0.4f);

    float crackle=0;
    if(ag_rng_next_f32(&f->rng) < f->crackle_density * 0.02f){
        float c = ag_noise_white(&f->noise) * 3.0f;
        c = c*c*c; /* sharp */
        c = ag_biquad_process(&f->bp, c);
        crackle = c * 0.8f;
    }
    return (base*0.7f + crackle*0.6f) * f->gain;
}

void ag_fireplace_init(AgFireplace *fp, double sr) {
    memset(fp,0,sizeof(*fp));
    ag_fire_init(&fp->fire, sr);
    fp->fire.gain=0.4f;
    ag_biquad_init(&fp->room_lp); ag_biquad_set(&fp->room_lp, AG_FILTER_LP, 1200.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&fp->room_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&fp->room_lfo, 0.2f);
    fp->room_gain=0.3f;
}
float ag_fireplace_next(AgFireplace *fp) {
    float fire = ag_fire_next(&fp->fire);
    float room = ag_biquad_process(&fp->room_lp, fire);
    float lfo = ag_osc_next(&fp->room_lfo)*0.1f;
    return (fire*0.6f + room*(0.4f+lfo)) * 0.9f;
}

void ag_torch_init(AgTorch *t, double sr) {
    memset(t,0,sizeof(*t));
    ag_fire_init(&t->fire, sr);
    t->fire.gain=0.35f;
    t->fire.crackle_density=0.08f;
    ag_osc_init(&t->wind_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&t->wind_lfo, 1.2f);
}
float ag_torch_next(AgTorch *t) {
    float fire = ag_fire_next(&t->fire);
    float wind = ag_osc_next(&t->wind_lfo)*0.2f;
    return fire * (1.0f + wind);
}

void ag_bonfire_init(AgBonfire *bf, double sr) {
    memset(bf,0,sizeof(*bf));
    ag_fire_init(&bf->fire, sr);
    bf->fire.gain=0.7f;
    bf->fire.base_intensity=0.8f;
    ag_noise_init(&bf->low_rumble, 0xB011);
    ag_biquad_init(&bf->rumble_lp); ag_biquad_set(&bf->rumble_lp, AG_FILTER_LP, 120.0f, 0.7f, 0, (float)sr);
}
float ag_bonfire_next(AgBonfire *bf) {
    float fire = ag_fire_next(&bf->fire);
    float rumble = ag_noise_brown(&bf->low_rumble) * 0.3f;
    rumble = ag_biquad_process(&bf->rumble_lp, rumble);
    return fire*0.7f + rumble*0.5f;
}
