#include "ag_biome.h"
#include <string.h>
#include <math.h>

void ag_biome_params_default(AgBiomeParams *p, AgBiomeType type) {
    memset(p,0,sizeof(*p));
    p->type=type;
    p->seed=1;
    p->time_of_day=0.5f;
    p->humidity=0.5f;
    p->weather_type=AG_WEATHER_CLEAR;
    p->weather=0.0f;
    switch(type){
        case AG_BIOME_FOREST:
            p->wind=0.3f; p->water=0.2f; p->birds=0.7f; p->insects=0.5f; p->humidity=0.6f;
            break;
        case AG_BIOME_CAVE:
            p->wind=0.1f; p->water=0.6f; p->birds=0.0f; p->insects=0.1f; p->humidity=0.9f;
            break;
        case AG_BIOME_DESERT:
            p->wind=0.6f; p->water=0.0f; p->birds=0.1f; p->insects=0.3f; p->humidity=0.1f;
            break;
        case AG_BIOME_OCEAN:
            p->wind=0.5f; p->water=1.0f; p->birds=0.3f; p->insects=0.0f; p->humidity=0.8f;
            break;
        case AG_BIOME_CITY:
            p->wind=0.2f; p->water=0.1f; p->birds=0.2f; p->insects=0.0f; p->humidity=0.4f;
            break;
        case AG_BIOME_MOUNTAIN:
            p->wind=0.7f; p->water=0.2f; p->birds=0.3f; p->insects=0.1f; p->humidity=0.3f;
            break;
        case AG_BIOME_JUNGLE:
            p->wind=0.2f; p->water=0.5f; p->birds=0.9f; p->insects=0.9f; p->humidity=0.9f;
            break;
        case AG_BIOME_SWAMP:
            p->wind=0.15f; p->water=0.7f; p->birds=0.3f; p->insects=0.8f; p->humidity=0.9f;
            break;
        case AG_BIOME_TUNDRA:
            p->wind=0.8f; p->water=0.1f; p->birds=0.1f; p->insects=0.0f; p->humidity=0.2f;
            break;
        case AG_BIOME_GRASSLAND:
            p->wind=0.4f; p->water=0.1f; p->birds=0.6f; p->insects=0.6f; p->humidity=0.4f;
            break;
        case AG_BIOME_RIVER:
            p->wind=0.2f; p->water=0.9f; p->birds=0.5f; p->insects=0.4f; p->humidity=0.6f;
            break;
        case AG_BIOME_BEACH:
            p->wind=0.4f; p->water=0.8f; p->birds=0.4f; p->insects=0.2f; p->humidity=0.7f;
            break;
        case AG_BIOME_NEXUS:
            p->wind=0.3f; p->water=0.3f; p->birds=0.2f; p->insects=0.2f; p->humidity=0.5f;
            break;
        case AG_BIOME_RIFT:
            p->wind=0.5f; p->water=0.1f; p->birds=0.0f; p->insects=0.0f; p->humidity=0.3f;
            break;
        case AG_BIOME_LAB:
            p->wind=0.05f; p->water=0.0f; p->birds=0.0f; p->insects=0.0f; p->humidity=0.2f;
            break;
        default: break;
    }
}

void ag_biome_init(AgBiome *biome, const AgBiomeParams *params, double sr) {
    memset(biome,0,sizeof(*biome));
    biome->sr=sr>0?sr:AG_SR_DEFAULT;
    biome->params=*params;
    biome->gain=1.0f;
    ag_rng_seed(&biome->rng, params->seed?params->seed:1);

    ag_wind_sys_init(&biome->wind, sr);
    ag_wind_sys_set(&biome->wind, params->wind*0.5f, params->wind*0.6f, params->wind*0.4f);

    ag_ocean_init(&biome->ocean, sr);
    ag_ocean_set(&biome->ocean, params->water, 0.4f);

    ag_river_init(&biome->river, sr);
    ag_stream_init(&biome->stream, sr);
    ag_waterfall_init(&biome->waterfall, sr);
    ag_drip_init(&biome->drip, sr);

    ag_fire_init(&biome->fire, sr);

    for(int i=0;i<4;i++) ag_bird_init(&biome->birds[i], sr, i%4);
    for(int i=0;i<2;i++) ag_cricket_init(&biome->crickets[i], sr);
    for(int i=0;i<2;i++) ag_cicada_init(&biome->cicadas[i], sr);
    for(int i=0;i<2;i++) ag_frog_init(&biome->frogs[i], sr);
    for(int i=0;i<1;i++) ag_owl_init(&biome->owls[i], sr);

    ag_swarm_init(&biome->swarm, sr);
    ag_weather_mixer_init(&biome->weather, sr);
    ag_weather_mixer_set(&biome->weather, params->weather_type, params->weather);

    ag_drone_init(&biome->drone, sr, 55.0f);
    ag_granular_init(&biome->granular, sr, 110.0f);

    /* Adjust based on time of day */
    float tod = params->time_of_day;
    /* Night = more crickets, owls, less birds */
    /* Day = more birds */
}

void ag_biome_set_params(AgBiome *biome, const AgBiomeParams *params) {
    biome->params=*params;
    ag_wind_sys_set(&biome->wind, params->wind*0.5f, params->wind*0.6f, params->wind*0.4f);
    ag_ocean_set(&biome->ocean, params->water, 0.4f);
    ag_weather_mixer_set(&biome->weather, params->weather_type, params->weather);
}

float ag_biome_next(AgBiome *biome) {
    float l,r; ag_biome_next_stereo(biome,&l,&r); return (l+r)*0.5f;
}
void ag_biome_next_stereo(AgBiome *biome, float *out_l, float *out_r) {
    float mix_l=0, mix_r=0;

    /* Wind always */
    float wind = ag_wind_sys_next(&biome->wind) * biome->params.wind;
    mix_l+=wind*0.3f; mix_r+=wind*0.3f;

    /* Water based on biome */
    if(biome->params.water > 0.01f){
        float water_l=0, water_r=0;
        switch(biome->params.type){
            case AG_BIOME_OCEAN:
            case AG_BIOME_BEACH:
                { float o=ag_ocean_next(&biome->ocean); water_l=o*0.5f; water_r=o*0.5f; }
                break;
            case AG_BIOME_RIVER:
                { float r=ag_river_next(&biome->river); water_l=r*0.5f; water_r=r*0.5f; }
                break;
            case AG_BIOME_CAVE:
                { ag_drip_auto(&biome->drip, biome->params.water); float d=ag_drip_next(&biome->drip); water_l=d*0.5f; water_r=d*0.5f; }
                break;
            default:
                { float s=ag_stream_next(&biome->stream); water_l=s*0.4f; water_r=s*0.4f; }
                break;
        }
        mix_l+=water_l*biome->params.water;
        mix_r+=water_r*biome->params.water;
    }

    /* Birds */
    if(biome->params.birds > 0.01f){
        float bird_mix=0;
        for(int i=0;i<4;i++){
            ag_bird_auto(&biome->birds[i], biome->params.birds * (0.5f + biome->params.time_of_day*0.5f));
            bird_mix+=ag_bird_next(&biome->birds[i]);
        }
        mix_l+=bird_mix*0.2f*biome->params.birds;
        mix_r+=bird_mix*0.2f*biome->params.birds;
    }

    /* Insects */
    if(biome->params.insects > 0.01f){
        float insect_l=0,insect_r=0;
        /* Day insects: cicada, swarm; Night: crickets */
        float day_factor = sinf(biome->params.time_of_day * (float)AG_PI); /* 0 at midnight, 1 at noon */
        float night_factor = 1.0f - day_factor;

        if(day_factor>0.2f){
            for(int i=0;i<2;i++) { float c=ag_cicada_next(&biome->cicadas[i]); insect_l+=c*0.15f; insect_r+=c*0.15f; }
            ag_swarm_next_stereo(&biome->swarm, &insect_l, &insect_r);
        }
        if(night_factor>0.2f){
            for(int i=0;i<2;i++) { float c=ag_cricket_next(&biome->crickets[i]); insect_l+=c*0.15f; insect_r+=c*0.15f; }
        }
        /* Frogs in swamp/jungle at night */
        if(biome->params.type==AG_BIOME_SWAMP || biome->params.type==AG_BIOME_JUNGLE){
            for(int i=0;i<2;i++) { float f=ag_frog_next(&biome->frogs[i]); insect_l+=f*0.2f; insect_r+=f*0.2f; }
        }
        /* Owls at night */
        if(night_factor>0.5f){
            for(int i=0;i<1;i++) { float o=ag_owl_next(&biome->owls[i]); insect_l+=o*0.2f; insect_r+=o*0.2f; }
        }

        mix_l+=insect_l*biome->params.insects;
        mix_r+=insect_r*biome->params.insects;
    }

    /* Fire */
    if(biome->params.fire > 0.01f){
        float f=ag_fire_next(&biome->fire);
        mix_l+=f*biome->params.fire*0.5f;
        mix_r+=f*biome->params.fire*0.5f;
    }

    /* Weather */
    if(biome->params.weather > 0.01f){
        float wl,wr; ag_weather_mixer_next_stereo(&biome->weather, &wl, &wr);
        mix_l+=wl*biome->params.weather;
        mix_r+=wr*biome->params.weather;
    }

    /* Drone for cave/nexus/rift */
    if(biome->params.type==AG_BIOME_CAVE || biome->params.type==AG_BIOME_NEXUS || biome->params.type==AG_BIOME_RIFT){
        float d=ag_drone_next(&biome->drone) * 0.3f;
        mix_l+=d; mix_r+=d;
    }
    if(biome->params.type==AG_BIOME_NEXUS){
        float g=ag_granular_next(&biome->granular) * 0.2f;
        mix_l+=g; mix_r+=g;
    }

    /* Soft clip */
    mix_l = ag_soft_clip(mix_l);
    mix_r = ag_soft_clip(mix_r);

    *out_l = mix_l * biome->gain;
    *out_r = mix_r * biome->gain;
}
void ag_biome_render(AgBiome *biome, float *stereo_interleaved, int frames) {
    for(int i=0;i<frames;i++){
        float l,r; ag_biome_next_stereo(biome,&l,&r);
        stereo_interleaved[i*2]=l;
        stereo_interleaved[i*2+1]=r;
    }
}

AgBiomeType ag_biome_from_string(const char *str) {
    if(!str) return AG_BIOME_FOREST;
    if(strcmp(str,"forest")==0) return AG_BIOME_FOREST;
    if(strcmp(str,"cave")==0) return AG_BIOME_CAVE;
    if(strcmp(str,"desert")==0) return AG_BIOME_DESERT;
    if(strcmp(str,"ocean")==0) return AG_BIOME_OCEAN;
    if(strcmp(str,"city")==0) return AG_BIOME_CITY;
    if(strcmp(str,"mountain")==0) return AG_BIOME_MOUNTAIN;
    if(strcmp(str,"jungle")==0) return AG_BIOME_JUNGLE;
    if(strcmp(str,"swamp")==0) return AG_BIOME_SWAMP;
    if(strcmp(str,"tundra")==0) return AG_BIOME_TUNDRA;
    if(strcmp(str,"grassland")==0) return AG_BIOME_GRASSLAND;
    if(strcmp(str,"river")==0) return AG_BIOME_RIVER;
    if(strcmp(str,"beach")==0) return AG_BIOME_BEACH;
    if(strcmp(str,"nexus")==0) return AG_BIOME_NEXUS;
    if(strcmp(str,"rift")==0) return AG_BIOME_RIFT;
    if(strcmp(str,"lab")==0) return AG_BIOME_LAB;
    /* scene names from Chrono Nexus */
    if(strcmp(str,"classroom")==0) return AG_BIOME_CITY;
    if(strcmp(str,"grove")==0) return AG_BIOME_FOREST;
    if(strcmp(str,"shore")==0) return AG_BIOME_BEACH;
    if(strcmp(str,"sanctum")==0) return AG_BIOME_CAVE;
    if(strcmp(str,"core")==0) return AG_BIOME_CAVE;
    if(strcmp(str,"festival")==0) return AG_BIOME_CITY;
    if(strcmp(str,"alley")==0) return AG_BIOME_CITY;
    if(strcmp(str,"lighthouse")==0) return AG_BIOME_OCEAN;
    return AG_BIOME_FOREST;
}
const char* ag_biome_to_string(AgBiomeType type) {
    switch(type){
        case AG_BIOME_FOREST: return "forest";
        case AG_BIOME_CAVE: return "cave";
        case AG_BIOME_DESERT: return "desert";
        case AG_BIOME_OCEAN: return "ocean";
        case AG_BIOME_CITY: return "city";
        case AG_BIOME_MOUNTAIN: return "mountain";
        case AG_BIOME_JUNGLE: return "jungle";
        case AG_BIOME_SWAMP: return "swamp";
        case AG_BIOME_TUNDRA: return "tundra";
        case AG_BIOME_GRASSLAND: return "grassland";
        case AG_BIOME_RIVER: return "river";
        case AG_BIOME_BEACH: return "beach";
        case AG_BIOME_NEXUS: return "nexus";
        case AG_BIOME_RIFT: return "rift";
        case AG_BIOME_LAB: return "lab";
        default: return "forest";
    }
}
