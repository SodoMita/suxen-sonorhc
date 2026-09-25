#ifndef AG_AMBIENCE_3D_H
#define AG_AMBIENCE_3D_H

#include "ag_common.h"
#include "ag_3d.h"
#include "ag_biome.h"
#include "ag_weather.h"
#include "ag_reverb.h"
#include "ag_delay.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3D Ambience System - places multiple environment sound sources in 3D space,
 * with listener, reverb zones, occlusion, and dynamic weather/time.
 */

#define AG_AMB_3D_MAX_LAYERS 8
#define AG_AMB_3D_MAX_POINT_SOURCES 16

typedef enum {
    AG_AMB_LAYER_BED = 0,      /* non-spatialized stereo bed (e.g., wind) */
    AG_AMB_LAYER_POINT,        /* 3D point source (e.g., bird, fire) */
    AG_AMB_LAYER_ZONE,         /* area sound (e.g., river, ocean) */
    AG_AMB_LAYER_REVERB        /* reverb return */
} AgAmbLayerType;

typedef struct AgAmbLayer {
    AgAmbLayerType type;
    int active;
    float gain;
    float gain_target;
    float gain_step;
    /* For point sources */
    AgSource source;
    AgSpatializer spatializer;
    /* For beds: simple gain */
    /* For zones: position + radius + biome */
    AgBiome biome;
    int has_biome;
    AgVec3 zone_pos;
    float zone_radius;
    /* Generator state - we use generic function pointer via enum */
    int gen_type; /* 0=wind,1=ocean,2=river,3=fire,4=birds,etc */
    void *gen_state; /* not used, we store directly in biome */
} AgAmbLayer;

typedef struct AgAmbience3D {
    AgListener listener;
    AgAmbLayer layers[AG_AMB_3D_MAX_LAYERS];
    AgSource point_sources[AG_AMB_3D_MAX_POINT_SOURCES];
    AgSpatializer point_spats[AG_AMB_3D_MAX_POINT_SOURCES];
    float point_gains[AG_AMB_3D_MAX_POINT_SOURCES];
    int point_active[AG_AMB_3D_MAX_POINT_SOURCES];
    int point_count;

    AgReverb reverb;
    AgReverbZone reverb_zones[4];
    int reverb_zone_count;

    AgBiomeParams biome_params;
    AgWeatherMixer weather;

    double sr;
    float master_gain;
    float master_target;
    float master_step;

    double time; /* sec elapsed */
    float time_of_day; /* 0..1 */
    float weather_intensity;

    AgRng rng;
} AgAmbience3D;

void ag_ambience_3d_init(AgAmbience3D *amb, double sr, AgVec3 listener_pos);
void ag_ambience_3d_set_listener(AgAmbience3D *amb, AgVec3 pos, AgVec3 forward, AgVec3 up, AgVec3 vel);
void ag_ambience_3d_set_biome(AgAmbience3D *amb, AgBiomeType biome_type, float time_of_day, float weather_intensity);
void ag_ambience_3d_set_weather(AgAmbience3D *amb, AgWeatherType weather_type, float intensity);

int ag_ambience_3d_add_point_source(AgAmbience3D *amb, AgVec3 pos, float min_dist, float max_dist, float gain);
void ag_ambience_3d_set_point_pos(AgAmbience3D *amb, int idx, AgVec3 pos);
void ag_ambience_3d_set_point_gain(AgAmbience3D *amb, int idx, float gain);
void ag_ambience_3d_remove_point_source(AgAmbience3D *amb, int idx);

void ag_ambience_3d_add_reverb_zone(AgAmbience3D *amb, AgVec3 pos, float radius, float reverb_gain, float damping, float room_size);

void ag_ambience_3d_set_master_gain(AgAmbience3D *amb, float gain, float fade_sec);

void ag_ambience_3d_render(AgAmbience3D *amb, float *stereo_interleaved, int frames);
float ag_ambience_3d_next(AgAmbience3D *amb, float *out_l, float *out_r);

/* Helper: create common ambiences */
void ag_ambience_3d_preset_forest(AgAmbience3D *amb);
void ag_ambience_3d_preset_cave(AgAmbience3D *amb);
void ag_ambience_3d_preset_ocean(AgAmbience3D *amb);
void ag_ambience_3d_preset_city(AgAmbience3D *amb);
void ag_ambience_3d_preset_nexus(AgAmbience3D *amb);
void ag_ambience_3d_preset_for_scene(AgAmbience3D *amb, const char *scene_name);

#ifdef __cplusplus
}
#endif

#endif
