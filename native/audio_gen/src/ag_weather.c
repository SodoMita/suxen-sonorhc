#include "ag_weather.h"
#include <string.h>
#include <math.h>

void ag_rain_sys_init(AgRainSystem *rs, double sr) {
    memset(rs,0,sizeof(*rs));
    rs->sr=sr>0?sr:AG_SR_DEFAULT;
    rs->gain=0.5f;
    rs->density=0.3f;
    rs->intensity=0.5f;
    rs->type=AG_WEATHER_RAIN_MEDIUM;
    ag_noise_init(&rs->noise, 0xA11A);
    ag_rng_seed(&rs->rng, 0xA11A);
    ag_biquad_init(&rs->lp); ag_biquad_set(&rs->lp, AG_FILTER_LP, 4000.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&rs->bp); ag_biquad_set(&rs->bp, AG_FILTER_BP, 2500.0f, 1.5f, 0, (float)sr);
}
void ag_rain_sys_set(AgRainSystem *rs, AgWeatherType type, float intensity) {
    rs->type=type;
    rs->intensity=ag_clamp_f(intensity,0,1);
    switch(type){
        case AG_WEATHER_RAIN_LIGHT: rs->density=0.15f; rs->gain=0.25f; break;
        case AG_WEATHER_RAIN_MEDIUM: rs->density=0.35f; rs->gain=0.45f; break;
        case AG_WEATHER_RAIN_HEAVY: rs->density=0.7f; rs->gain=0.7f; break;
        case AG_WEATHER_THUNDERSTORM: rs->density=0.6f; rs->gain=0.6f; break;
        case AG_WEATHER_HAIL: rs->density=0.5f; rs->gain=0.6f; break;
        case AG_WEATHER_SNOW_LIGHT: rs->density=0.1f; rs->gain=0.15f; break;
        case AG_WEATHER_SNOW_HEAVY: rs->density=0.25f; rs->gain=0.3f; break;
        default: rs->density=0.0f; rs->gain=0.0f; break;
    }
    rs->gain *= (0.5f + rs->intensity*0.5f);
}
float ag_rain_sys_next(AgRainSystem *rs) {
    if(rs->density<=0.001f) return 0;
    float out=0;
    /* continuous drizzle */
    float drizzle = ag_noise_white(&rs->noise) * 0.03f * rs->intensity;
    drizzle = ag_biquad_process(&rs->lp, drizzle);
    out+=drizzle;

    /* random drops */
    rs->drip_timer+=1.0/rs->sr;
    if(rs->drip_timer > 0.005){
        rs->drip_timer=0;
        if(ag_rng_next_f32(&rs->rng) < rs->density*0.15f){
            float drop = ag_rng_range_f32(&rs->rng, 0.2f, 1.0f) * rs->intensity;
            float freq = ag_rng_range_f32(&rs->rng, 1500, 6000);
            if(rs->type==AG_WEATHER_HAIL) freq = ag_rng_range_f32(&rs->rng, 2000, 8000);
            if(rs->type==AG_WEATHER_SNOW_LIGHT || rs->type==AG_WEATHER_SNOW_HEAVY) freq = ag_rng_range_f32(&rs->rng, 800, 2000);
            ag_biquad_set(&rs->bp, AG_FILTER_BP, freq, 2.0f, 0, (float)rs->sr);
            float n = ag_noise_white(&rs->noise) * drop;
            n = ag_biquad_process(&rs->bp, n);
            out+=n*0.9f;
        }
    }
    return out * rs->gain;
}

void ag_thunder_init(AgThunder *th, double sr) {
    memset(th,0,sizeof(*th));
    th->sr=sr>0?sr:AG_SR_DEFAULT;
    th->gain=0.8f;
    ag_noise_init(&th->noise, 0x7414);
    ag_biquad_init(&th->lp); ag_biquad_set(&th->lp, AG_FILTER_LP, 200.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&th->bp); ag_biquad_set(&th->bp, AG_FILTER_BP, 80.0f, 1.2f, 0, (float)sr);
    ag_osc_init(&th->rumble_lfo, AG_OSC_SINE, sr);
    ag_osc_set_freq(&th->rumble_lfo, 0.08f);
    ag_rng_seed(&th->rng, 0x7414);
    th->next_thunder=5.0;
    AgADSR adsr={0.05f,1.5f,0.0f,0.8f,1,1,1};
    ag_env_init(&th->env, adsr, sr);
}
void ag_thunder_trigger(AgThunder *th) {
    ag_env_trigger(&th->env);
    th->active=1;
    th->active_time=0;
    th->active_dur=ag_rng_range_f32(&th->rng, 0.8f, 2.5f);
}
float ag_thunder_next(AgThunder *th) {
    th->timer+=1.0/th->sr;
    if(th->active){
        th->active_time+=1.0/th->sr;
        if(th->active_time >= th->active_dur) th->active=0;
    }
    if(!th->active) return 0;
    float env=ag_env_next(&th->env);
    if(ag_env_is_idle(&th->env)) th->active=0;
    float n = ag_noise_brown(&th->noise) * 0.6f + ag_noise_white(&th->noise)*0.2f;
    n = ag_biquad_process(&th->lp, n);
    n = ag_biquad_process(&th->bp, n);
    float rumble = ag_osc_next(&th->rumble_lfo)*0.3f;
    n *= (1.0f + rumble);
    return n * env * th->gain * 1.2f;
}
void ag_thunder_auto(AgThunder *th, float storm_intensity) {
    if(th->timer >= th->next_thunder){
        th->timer=0;
        th->next_thunder = ag_rng_range_f32(&th->rng, 3.0f, 12.0f) / (storm_intensity>0.01f?storm_intensity:0.01f);
        if(ag_rng_next_f32(&th->rng) < storm_intensity*0.5f) ag_thunder_trigger(th);
    }
}

void ag_wind_sys_init(AgWindSystem *ws, double sr) {
    memset(ws,0,sizeof(*ws));
    ws->sr=sr>0?sr:AG_SR_DEFAULT;
    ws->gain=0.5f;
    ws->base_strength=0.3f;
    ws->gust_strength=0.3f;
    ws->turbulence=0.2f;
    ag_noise_init(&ws->noise, 0x1111);
    ag_biquad_init(&ws->lp1); ag_biquad_set(&ws->lp1, AG_FILTER_LP, 800.0f, 0.7f, 0, (float)sr);
    ag_biquad_init(&ws->lp2); ag_biquad_set(&ws->lp2, AG_FILTER_LP, 400.0f, 0.7f, 0, (float)sr);
    ag_osc_init(&ws->gust_lfo1, AG_OSC_SINE, sr); ag_osc_set_freq(&ws->gust_lfo1, 0.11f);
    ag_osc_init(&ws->gust_lfo2, AG_OSC_SINE, sr); ag_osc_set_freq(&ws->gust_lfo2, 0.23f);
    ag_osc_init(&ws->turbulence_lfo, AG_OSC_SINE, sr); ag_osc_set_freq(&ws->turbulence_lfo, 1.5f);
    ag_rng_seed(&ws->rng, 0x1111);
}
void ag_wind_sys_set(AgWindSystem *ws, float base_strength, float gust_strength, float turbulence) {
    ws->base_strength=ag_clamp_f(base_strength,0,1);
    ws->gust_strength=ag_clamp_f(gust_strength,0,1);
    ws->turbulence=ag_clamp_f(turbulence,0,1);
}
float ag_wind_sys_next(AgWindSystem *ws) {
    float white = ag_noise_white(&ws->noise);
    float pink = ag_noise_pink(&ws->noise);
    float n = white*0.25f + pink*0.75f;
    n = ag_biquad_process(&ws->lp1, n);
    n = ag_biquad_process(&ws->lp2, n);
    float gust1 = ag_osc_next(&ws->gust_lfo1);
    float gust2 = ag_osc_next(&ws->gust_lfo2);
    float turb = ag_osc_next(&ws->turbulence_lfo) * ws->turbulence;
    float env = ws->base_strength + (gust1*0.5f+gust2*0.3f)*ws->gust_strength + turb*0.2f;
    env = ag_clamp_f(env, 0, 1.2f);
    return n * env * ws->gain;
}

void ag_weather_mixer_init(AgWeatherMixer *wm, double sr) {
    memset(wm,0,sizeof(*wm));
    wm->sr=sr>0?sr:AG_SR_DEFAULT;
    wm->gain=0.7f;
    wm->type=AG_WEATHER_CLEAR;
    wm->intensity=0.0f;
    ag_rain_sys_init(&wm->rain, sr);
    ag_thunder_init(&wm->thunder, sr);
    ag_wind_sys_init(&wm->wind, sr);
}
void ag_weather_mixer_set(AgWeatherMixer *wm, AgWeatherType type, float intensity) {
    wm->type=type;
    wm->intensity=ag_clamp_f(intensity,0,1);
    ag_rain_sys_set(&wm->rain, type, intensity);
    if(type==AG_WEATHER_THUNDERSTORM) ag_wind_sys_set(&wm->wind, 0.4f*intensity, 0.6f*intensity, 0.5f*intensity);
    else if(type==AG_WEATHER_WINDY) ag_wind_sys_set(&wm->wind, 0.5f*intensity, 0.7f*intensity, 0.6f*intensity);
    else if(type==AG_WEATHER_CLEAR) ag_wind_sys_set(&wm->wind, 0.05f, 0.05f, 0.05f);
    else ag_wind_sys_set(&wm->wind, 0.1f*intensity, 0.2f*intensity, 0.1f*intensity);
}
float ag_weather_mixer_next(AgWeatherMixer *wm) {
    float l,r; ag_weather_mixer_next_stereo(wm,&l,&r); return (l+r)*0.5f;
}
void ag_weather_mixer_next_stereo(AgWeatherMixer *wm, float *out_l, float *out_r) {
    float rain = ag_rain_sys_next(&wm->rain);
    float wind = ag_wind_sys_next(&wm->wind);
    float thunder = 0;
    if(wm->type==AG_WEATHER_THUNDERSTORM){
        ag_thunder_auto(&wm->thunder, wm->intensity);
        thunder = ag_thunder_next(&wm->thunder);
    }
    float mix_l = rain*0.5f + wind*0.5f + thunder*0.7f;
    float mix_r = rain*0.5f + wind*0.5f + thunder*0.7f;
    /* slight stereo for wind */
    mix_l += wind*0.1f;
    mix_r -= wind*0.05f;
    *out_l = mix_l * wm->gain;
    *out_r = mix_r * wm->gain;
}
