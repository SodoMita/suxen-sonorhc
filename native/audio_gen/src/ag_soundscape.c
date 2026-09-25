#include "ag_soundscape.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

void ag_soundscape_params_default(AgSoundscapeParams *p) {
    memset(p,0,sizeof(*p));
    p->biome=AG_BIOME_FOREST;
    p->mood=AG_MOOD_CALM;
    p->weather=AG_WEATHER_CLEAR;
    p->weather_intensity=0.0f;
    p->time_of_day=0.5f;
    p->music_gain=0.6f;
    p->ambience_gain=1.0f;
    p->master_gain=1.0f;
    p->seed=1;
    p->use_3d=1;
    p->use_music=1;
}
void ag_soundscape_params_for_scene(const char *scene_name, AgSoundscapeParams *p, uint64_t seed) {
    ag_soundscape_params_default(p);
    p->seed=seed?seed:1;
    if(!scene_name) scene_name="classroom";
    if(strcmp(scene_name,"classroom")==0){
        p->biome=AG_BIOME_CITY; p->mood=AG_MOOD_CALM; p->time_of_day=0.6f; p->music_gain=0.4f; p->ambience_gain=0.3f;
    } else if(strcmp(scene_name,"grove")==0){
        p->biome=AG_BIOME_FOREST; p->mood=AG_MOOD_DREAM; p->time_of_day=0.5f; p->music_gain=0.45f; p->ambience_gain=0.6f;
    } else if(strcmp(scene_name,"shore")==0){
        p->biome=AG_BIOME_BEACH; p->mood=AG_MOOD_WARM; p->time_of_day=0.6f; p->music_gain=0.4f; p->ambience_gain=0.7f;
    } else if(strcmp(scene_name,"nexus")==0){
        p->biome=AG_BIOME_NEXUS; p->mood=AG_MOOD_TENSE; p->time_of_day=0.5f; p->music_gain=0.5f; p->ambience_gain=0.6f; p->use_3d=1;
    } else if(strcmp(scene_name,"rift")==0){
        p->biome=AG_BIOME_RIFT; p->mood=AG_MOOD_RIFT; p->time_of_day=0.2f; p->music_gain=0.6f; p->ambience_gain=0.5f; p->use_3d=1;
    } else if(strcmp(scene_name,"core")==0){
        p->biome=AG_BIOME_CAVE; p->mood=AG_MOOD_TENSE; p->time_of_day=0.0f; p->music_gain=0.55f; p->ambience_gain=0.5f;
    } else if(strcmp(scene_name,"lab")==0){
        p->biome=AG_BIOME_LAB; p->mood=AG_MOOD_LAB; p->time_of_day=0.7f; p->music_gain=0.5f; p->ambience_gain=0.2f;
    } else if(strcmp(scene_name,"festival")==0){
        p->biome=AG_BIOME_CITY; p->mood=AG_MOOD_FESTIVAL; p->time_of_day=0.8f; p->music_gain=0.6f; p->ambience_gain=0.5f;
    } else if(strcmp(scene_name,"sanctum")==0){
        p->biome=AG_BIOME_CAVE; p->mood=AG_MOOD_AMBIENT; p->time_of_day=0.3f; p->music_gain=0.5f; p->ambience_gain=0.6f;
    } else if(strcmp(scene_name,"alley")==0){
        p->biome=AG_BIOME_CITY; p->mood=AG_MOOD_NIGHT; p->time_of_day=0.1f; p->music_gain=0.45f; p->ambience_gain=0.4f; p->weather=AG_WEATHER_RAIN_LIGHT; p->weather_intensity=0.3f;
    } else if(strcmp(scene_name,"lighthouse")==0){
        p->biome=AG_BIOME_OCEAN; p->mood=AG_MOOD_CALM; p->time_of_day=0.4f; p->music_gain=0.4f; p->ambience_gain=0.7f;
    }
}

void ag_soundscape_init(AgSoundscape *ss, const AgSoundscapeParams *params, double sr) {
    memset(ss,0,sizeof(*ss));
    ss->sr=sr>0?sr:AG_SR_DEFAULT;
    ss->params=*params;
    ss->gain=params->master_gain;
    ag_rng_seed(&ss->rng, params->seed?params->seed:1);

    ag_ambience_3d_init(&ss->ambience_3d, sr, ag_vec3(0,0,0));
    ag_ambience_3d_preset_for_scene(&ss->ambience_3d, ag_biome_to_string(params->biome));
    ag_ambience_3d_set_biome(&ss->ambience_3d, params->biome, params->time_of_day, params->weather_intensity);
    ag_ambience_3d_set_weather(&ss->ambience_3d, params->weather, params->weather_intensity);

    AgBiomeParams bp; ag_biome_params_default(&bp, params->biome);
    bp.time_of_day=params->time_of_day;
    bp.weather=params->weather;
    bp.weather_type=params->weather;
    bp.seed=params->seed;
    ag_biome_init(&ss->biome, &bp, sr);

    ag_proc_mixer_init(&ss->music_mixer, sr);
    if(params->use_music){
        AgProcSpec spec; ag_proc_spec_from_mood(&spec, params->mood, 60, 80, params->seed);
        spec.id=ag_rng_next_u64(&ss->rng);
        ag_proc_mixer_transition(&ss->music_mixer, &spec, 0.8f);
        ag_proc_mixer_set_gain(&ss->music_mixer, params->music_gain, 0.5f);
    }

    ag_weather_mixer_init(&ss->weather, sr);
    ag_weather_mixer_set(&ss->weather, params->weather, params->weather_intensity);
}
void ag_soundscape_set_params(AgSoundscape *ss, const AgSoundscapeParams *params, float fade_sec) {
    ss->params=*params;
    ag_ambience_3d_set_biome(&ss->ambience_3d, params->biome, params->time_of_day, params->weather_intensity);
    ag_ambience_3d_set_weather(&ss->ambience_3d, params->weather, params->weather_intensity);
    ag_biome_set_params(&ss->biome, &(AgBiomeParams){.type=params->biome, .time_of_day=params->time_of_day, .weather=params->weather, .weather_type=params->weather, .wind=params->biome==AG_BIOME_FOREST?0.3f:0.2f, .water=0.5f, .birds=0.5f, .insects=0.5f});
    if(params->use_music){
        AgProcSpec spec; ag_proc_spec_from_mood(&spec, params->mood, 60, 80, ag_rng_next_u64(&ss->rng));
        spec.id=ag_rng_next_u64(&ss->rng);
        ag_proc_mixer_transition(&ss->music_mixer, &spec, fade_sec);
    }
    ag_weather_mixer_set(&ss->weather, params->weather, params->weather_intensity);
    ag_ambience_3d_set_master_gain(&ss->ambience_3d, params->ambience_gain, fade_sec);
    ag_proc_mixer_set_gain(&ss->music_mixer, params->music_gain, fade_sec);
}
void ag_soundscape_set_time_of_day(AgSoundscape *ss, float tod, float fade_sec) {
    ss->params.time_of_day=ag_clamp_f(tod,0,1);
    ag_ambience_3d_set_biome(&ss->ambience_3d, ss->params.biome, tod, ss->params.weather_intensity);
    AgBiomeParams bp = ss->biome.params;
    bp.time_of_day=tod;
    ag_biome_set_params(&ss->biome, &bp);
    (void)fade_sec;
}
void ag_soundscape_set_weather(AgSoundscape *ss, AgWeatherType weather, float intensity, float fade_sec) {
    ss->params.weather=weather;
    ss->params.weather_intensity=ag_clamp_f(intensity,0,1);
    ag_ambience_3d_set_weather(&ss->ambience_3d, weather, intensity);
    ag_weather_mixer_set(&ss->weather, weather, intensity);
    (void)fade_sec;
}
void ag_soundscape_set_mood(AgSoundscape *ss, AgMood mood, float fade_sec) {
    ss->params.mood=mood;
    AgProcSpec spec; ag_proc_spec_from_mood(&spec, mood, 60, 80, ag_rng_next_u64(&ss->rng));
    spec.id=ag_rng_next_u64(&ss->rng);
    ag_proc_mixer_transition(&ss->music_mixer, &spec, fade_sec);
}

void ag_soundscape_render(AgSoundscape *ss, float *stereo_interleaved, int frames) {
    /* temp buffers */
    float *amb_buf = (float*)malloc(sizeof(float)*frames*2);
    float *music_buf = (float*)malloc(sizeof(float)*frames*2);
    float *weather_buf = (float*)malloc(sizeof(float)*frames*2);
    if(!amb_buf || !music_buf || !weather_buf){ free(amb_buf); free(music_buf); free(weather_buf); return; }

    if(ss->params.use_3d){
        ag_ambience_3d_render(&ss->ambience_3d, amb_buf, frames);
    } else {
        ag_biome_render(&ss->biome, amb_buf, frames);
    }

    if(ss->params.use_music){
        ag_proc_mixer_render(&ss->music_mixer, music_buf, frames);
    } else {
        memset(music_buf,0,sizeof(float)*frames*2);
    }

    for(int i=0;i<frames;i++){
        float wl,wr; ag_weather_mixer_next_stereo(&ss->weather, &wl, &wr);
        weather_buf[i*2]=wl;
        weather_buf[i*2+1]=wr;
    }

    for(int i=0;i<frames*2;i++){
        float amb = amb_buf[i] * ss->params.ambience_gain;
        float mus = music_buf[i] * ss->params.music_gain;
        float wea = weather_buf[i] * ss->params.weather_intensity * 0.5f;
        float out = amb + mus + wea;
        out = ag_soft_clip(out);
        stereo_interleaved[i]=out * ss->params.master_gain;
    }

    free(amb_buf); free(music_buf); free(weather_buf);
    ss->time+= (double)frames / ss->sr;
}
float ag_soundscape_next(AgSoundscape *ss, float *l, float *r) {
    float buf[2]; ag_soundscape_render(ss, buf, 1);
    *l=buf[0]; *r=buf[1];
    return (buf[0]+buf[1])*0.5f;
}

void ag_soundscape_preset_classroom(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("classroom",&p,1);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_grove(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("grove",&p,2);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_shore(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("shore",&p,3);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_nexus(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("nexus",&p,4);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_rift(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("rift",&p,5);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_lab(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("lab",&p,6);
    ag_soundscape_set_params(ss,&p,0.8f);
}
void ag_soundscape_preset_festival(AgSoundscape *ss) {
    AgSoundscapeParams p; ag_soundscape_params_for_scene("festival",&p,7);
    ag_soundscape_set_params(ss,&p,0.8f);
}
