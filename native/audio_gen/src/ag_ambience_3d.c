#include "ag_ambience_3d.h"
#include <string.h>
#include <math.h>

void ag_ambience_3d_init(AgAmbience3D *amb, double sr, AgVec3 listener_pos) {
    memset(amb,0,sizeof(*amb));
    amb->sr=sr>0?sr:AG_SR_DEFAULT;
    amb->master_gain=1.0f;
    amb->master_target=1.0f;
    ag_listener_init(&amb->listener, listener_pos);
    ag_reverb_init(&amb->reverb, (int)sr);
    ag_reverb_set_room_size(&amb->reverb, 0.5f);
    ag_reverb_set_damping(&amb->reverb, 0.5f);
    ag_reverb_set_wet(&amb->reverb, 0.25f);
    ag_reverb_set_dry(&amb->reverb, 0.85f);
    ag_weather_mixer_init(&amb->weather, sr);
    ag_biome_params_default(&amb->biome_params, AG_BIOME_FOREST);
    amb->time_of_day=0.5f;
    ag_rng_seed(&amb->rng, 0xA0B);
    for(int i=0;i<AG_AMB_3D_MAX_POINT_SOURCES;i++) ag_spatializer_init(&amb->point_spats[i], (int)sr);
}
void ag_ambience_3d_set_listener(AgAmbience3D *amb, AgVec3 pos, AgVec3 forward, AgVec3 up, AgVec3 vel) {
    ag_listener_update(&amb->listener, pos, vel);
    ag_listener_set_orientation(&amb->listener, forward, up);
}
void ag_ambience_3d_set_biome(AgAmbience3D *amb, AgBiomeType biome_type, float time_of_day, float weather_intensity) {
    ag_biome_params_default(&amb->biome_params, biome_type);
    amb->biome_params.time_of_day=ag_clamp_f(time_of_day,0,1);
    amb->biome_params.weather=ag_clamp_f(weather_intensity,0,1);
    amb->time_of_day=time_of_day;
    amb->weather_intensity=weather_intensity;
    /* Reconfigure layers */
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++){
        if(amb->layers[i].has_biome){
            ag_biome_set_params(&amb->layers[i].biome, &amb->biome_params);
        }
    }
}
void ag_ambience_3d_set_weather(AgAmbience3D *amb, AgWeatherType weather_type, float intensity) {
    amb->biome_params.weather_type=weather_type;
    amb->biome_params.weather=ag_clamp_f(intensity,0,1);
    ag_weather_mixer_set(&amb->weather, weather_type, intensity);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++){
        if(amb->layers[i].has_biome){
            amb->layers[i].biome.params.weather_type=weather_type;
            amb->layers[i].biome.params.weather=intensity;
            ag_weather_mixer_set(&amb->layers[i].biome.weather, weather_type, intensity);
        }
    }
}
int ag_ambience_3d_add_point_source(AgAmbience3D *amb, AgVec3 pos, float min_dist, float max_dist, float gain) {
    if(amb->point_count >= AG_AMB_3D_MAX_POINT_SOURCES) return -1;
    int idx=amb->point_count++;
    ag_source_init(&amb->point_sources[idx], pos);
    ag_source_set_dist(&amb->point_sources[idx], min_dist, max_dist, 1.0f, AG_DIST_INVERSE);
    amb->point_gains[idx]=gain;
    amb->point_active[idx]=1;
    return idx;
}
void ag_ambience_3d_set_point_pos(AgAmbience3D *amb, int idx, AgVec3 pos) {
    if(idx<0||idx>=amb->point_count) return;
    amb->point_sources[idx].pos=pos;
}
void ag_ambience_3d_set_point_gain(AgAmbience3D *amb, int idx, float gain) {
    if(idx<0||idx>=amb->point_count) return;
    amb->point_gains[idx]=gain;
}
void ag_ambience_3d_remove_point_source(AgAmbience3D *amb, int idx) {
    if(idx<0||idx>=amb->point_count) return;
    for(int i=idx;i<amb->point_count-1;i++){
        amb->point_sources[i]=amb->point_sources[i+1];
        amb->point_spats[i]=amb->point_spats[i+1];
        amb->point_gains[i]=amb->point_gains[i+1];
        amb->point_active[i]=amb->point_active[i+1];
    }
    amb->point_count--;
}
void ag_ambience_3d_add_reverb_zone(AgAmbience3D *amb, AgVec3 pos, float radius, float reverb_gain, float damping, float room_size) {
    if(amb->reverb_zone_count>=4) return;
    AgReverbZone *z=&amb->reverb_zones[amb->reverb_zone_count++];
    z->pos=pos; z->radius=radius; z->reverb_gain=reverb_gain; z->damping=damping; z->room_size=room_size;
}
void ag_ambience_3d_set_master_gain(AgAmbience3D *amb, float gain, float fade_sec) {
    amb->master_target=ag_clamp_f(gain,0,1.5f);
    float dist=fabsf(amb->master_target-amb->master_gain);
    if(fade_sec<=0.0001f) amb->master_step=dist>0?dist:1.0f;
    else amb->master_step=dist/(fade_sec*(float)amb->sr);
    if(amb->master_step<1e-12f) amb->master_step=1e-12f;
}
static void ramp_toward(float *v, float tgt, float step){
    if(*v<tgt){ *v+=step; if(*v>tgt) *v=tgt; }
    else if(*v>tgt){ *v-=step; if(*v<tgt) *v=tgt; }
}

float ag_ambience_3d_next(AgAmbience3D *amb, float *out_l, float *out_r) {
    float buf[2]; ag_ambience_3d_render(amb, buf, 1);
    *out_l=buf[0]; *out_r=buf[1];
    return (buf[0]+buf[1])*0.5f;
}
void ag_ambience_3d_render(AgAmbience3D *amb, float *stereo_interleaved, int frames) {
    for(int f=0; f<frames; f++){
        amb->time+=1.0/amb->sr;
        float mix_l=0, mix_r=0;

        /* Layers (biome beds) */
        for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++){
            AgAmbLayer *layer=&amb->layers[i];
            if(!layer->active) continue;
            ramp_toward(&layer->gain, layer->gain_target, layer->gain_step);
            float l=0,r=0;
            if(layer->has_biome){
                ag_biome_next_stereo(&layer->biome, &l, &r);
            } else {
                /* point layer - for now silence, point sources handled separately */
                continue;
            }
            mix_l+=l*layer->gain;
            mix_r+=r*layer->gain;
        }

        /* Point sources - for demo, we generate simple sine/noise per source as placeholder
         * In real use, in_mono_per_source would be provided externally.
         * Here we generate procedural per-source based on its index.
         */
        for(int s=0;s<amb->point_count;s++){
            if(!amb->point_active[s]) continue;
            /* Generate a simple procedural sound per source: e.g., fire crackle or bird */
            float src_mono = 0.0f;
            /* Use rng to vary */
            float t = (float)amb->time;
            /* Simple: sine at different freq + noise */
            src_mono = sinf(t * (200.0f + s*50.0f) * 0.1f) * 0.1f + (ag_rng_next_f32(&amb->rng)*0.02f);
            float l,r;
            ag_spatializer_process(&amb->point_spats[s], &amb->point_sources[s], &amb->listener, src_mono, &l, &r);
            mix_l+=l*amb->point_gains[s];
            mix_r+=r*amb->point_gains[s];
        }

        /* Weather */
        float wl,wr; ag_weather_mixer_next_stereo(&amb->weather, &wl, &wr);
        mix_l+=wl; mix_r+=wr;

        /* Reverb zones */
        float reverb_gain=0;
        for(int i=0;i<amb->reverb_zone_count;i++){
            reverb_gain+=ag_3d_reverb_zone_gain(&amb->reverb_zones[i], amb->listener.pos);
        }
        reverb_gain=ag_clamp_f(reverb_gain,0,1);
        if(reverb_gain>0.01f){
            ag_reverb_set_wet(&amb->reverb, reverb_gain*0.4f);
            float rev_l, rev_r;
            ag_reverb_process_stereo(&amb->reverb, mix_l, mix_r, &rev_l, &rev_r);
            mix_l=rev_l; mix_r=rev_r;
        }

        ramp_toward(&amb->master_gain, amb->master_target, amb->master_step);
        mix_l*=amb->master_gain;
        mix_r*=amb->master_gain;

        /* soft clip */
        mix_l = ag_soft_clip(mix_l);
        mix_r = ag_soft_clip(mix_r);

        stereo_interleaved[f*2]=mix_l;
        stereo_interleaved[f*2+1]=mix_r;
    }
}

void ag_ambience_3d_preset_forest(AgAmbience3D *amb) {
    ag_ambience_3d_set_biome(amb, AG_BIOME_FOREST, 0.5f, 0.0f);
    /* Add a biome layer */
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.6f; amb->layers[i].gain_target=0.6f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, AG_BIOME_FOREST);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
}
void ag_ambience_3d_preset_cave(AgAmbience3D *amb) {
    ag_ambience_3d_set_biome(amb, AG_BIOME_CAVE, 0.0f, 0.0f);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.5f; amb->layers[i].gain_target=0.5f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, AG_BIOME_CAVE);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
    ag_ambience_3d_add_reverb_zone(amb, ag_vec3(0,0,0), 20.0f, 0.8f, 0.6f, 0.9f);
}
void ag_ambience_3d_preset_ocean(AgAmbience3D *amb) {
    ag_ambience_3d_set_biome(amb, AG_BIOME_OCEAN, 0.5f, 0.1f);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.7f; amb->layers[i].gain_target=0.7f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, AG_BIOME_OCEAN);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
}
void ag_ambience_3d_preset_city(AgAmbience3D *amb) {
    ag_ambience_3d_set_biome(amb, AG_BIOME_CITY, 0.5f, 0.0f);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.4f; amb->layers[i].gain_target=0.4f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, AG_BIOME_CITY);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
}
void ag_ambience_3d_preset_nexus(AgAmbience3D *amb) {
    ag_ambience_3d_set_biome(amb, AG_BIOME_NEXUS, 0.5f, 0.0f);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.6f; amb->layers[i].gain_target=0.6f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, AG_BIOME_NEXUS);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
    ag_ambience_3d_add_reverb_zone(amb, ag_vec3(0,0,0), 30.0f, 0.6f, 0.4f, 0.7f);
}
void ag_ambience_3d_preset_for_scene(AgAmbience3D *amb, const char *scene_name) {
    AgBiomeType bt = ag_biome_from_string(scene_name);
    ag_ambience_3d_set_biome(amb, bt, 0.5f, 0.0f);
    for(int i=0;i<AG_AMB_3D_MAX_LAYERS;i++) if(!amb->layers[i].active){
        amb->layers[i].active=1;
        amb->layers[i].gain=0.6f; amb->layers[i].gain_target=0.6f;
        amb->layers[i].has_biome=1;
        AgBiomeParams p; ag_biome_params_default(&p, bt);
        p.seed=ag_rng_next_u64(&amb->rng);
        ag_biome_init(&amb->layers[i].biome, &p, amb->sr);
        break;
    }
    if(bt==AG_BIOME_CAVE || bt==AG_BIOME_NEXUS){
        ag_ambience_3d_add_reverb_zone(amb, ag_vec3(0,0,0), 25.0f, 0.7f, 0.5f, 0.8f);
    }
}
