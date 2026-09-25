#ifndef AG_3D_H
#define AG_3D_H

#include "ag_common.h"
#include "ag_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 3D Audio Spatialization - vector math + distance + panning + Doppler + air absorption */

typedef struct AgVec3 {
    float x, y, z;
} AgVec3;

static inline AgVec3 ag_vec3(float x, float y, float z) { AgVec3 v={x,y,z}; return v; }
static inline AgVec3 ag_vec3_add(AgVec3 a, AgVec3 b) { return ag_vec3(a.x+b.x, a.y+b.y, a.z+b.z); }
static inline AgVec3 ag_vec3_sub(AgVec3 a, AgVec3 b) { return ag_vec3(a.x-b.x, a.y-b.y, a.z-b.z); }
static inline AgVec3 ag_vec3_mul(AgVec3 a, float s) { return ag_vec3(a.x*s, a.y*s, a.z*s); }
static inline float ag_vec3_dot(AgVec3 a, AgVec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline float ag_vec3_len(AgVec3 a) { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z); }
static inline float ag_vec3_len2(AgVec3 a) { return a.x*a.x + a.y*a.y + a.z*a.z; }
static inline AgVec3 ag_vec3_cross(AgVec3 a, AgVec3 b) { return ag_vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }
static inline AgVec3 ag_vec3_norm(AgVec3 a) { float l=ag_vec3_len(a); if(l<1e-6f) return ag_vec3(0,0,0); return ag_vec3_mul(a, 1.0f/l); }
static inline float ag_vec3_dist(AgVec3 a, AgVec3 b) { return ag_vec3_len(ag_vec3_sub(a,b)); }

typedef enum {
    AG_DIST_LINEAR = 0,
    AG_DIST_INVERSE,
    AG_DIST_EXP,
    AG_DIST_NONE
} AgDistModel;

typedef struct AgListener {
    AgVec3 pos;
    AgVec3 vel;
    AgVec3 forward; /* normalized, look direction */
    AgVec3 up;
    AgVec3 right;   /* computed from forward x up */
    float speed_of_sound; /* m/s, default 343 */
    float doppler_factor; /* 0..1 */
    float air_absorption; /* 0..1 amount */
} AgListener;

typedef struct AgSource {
    AgVec3 pos;
    AgVec3 vel;
    float gain;       /* base gain */
    float min_dist;   /* distance where attenuation starts */
    float max_dist;   /* distance where sound is silent (linear) or min gain */
    float rolloff;    /* rolloff factor */
    AgDistModel dist_model;
    float cone_inner_angle; /* degrees, 360 = omni */
    float cone_outer_angle;
    float cone_outer_gain;
    AgVec3 cone_dir; /* direction of cone */
    int is_ambient; /* if true, less spatialized */
} AgSource;

void ag_listener_init(AgListener *lis, AgVec3 pos);
void ag_listener_set_orientation(AgListener *lis, AgVec3 forward, AgVec3 up);
void ag_listener_update(AgListener *lis, AgVec3 pos, AgVec3 vel);

void ag_source_init(AgSource *src, AgVec3 pos);
void ag_source_set_dist(AgSource *src, float min_dist, float max_dist, float rolloff, AgDistModel model);
void ag_source_set_cone(AgSource *src, AgVec3 dir, float inner_deg, float outer_deg, float outer_gain);

/* Attenuation 0..1 based on distance */
float ag_3d_attenuation(const AgSource *src, float distance);
float ag_3d_cone_gain(const AgSource *src, AgVec3 listener_pos);
float ag_3d_doppler_pitch(const AgSource *src, const AgListener *lis);

/* Panning */
typedef struct AgPanning {
    float l, r;       /* gains */
    float pan;        /* -1..1 */
    float itd_ms;     /* interaural time diff */
    float ild_db;     /* interaural level diff */
} AgPanning;

AgPanning ag_3d_pan_stereo(const AgSource *src, const AgListener *lis);
AgPanning ag_3d_pan_binaural(const AgSource *src, const AgListener *lis);

/* Air absorption - returns low-pass cutoff freq */
float ag_3d_air_absorption_fc(float distance, float base_fc);

/* Occlusion - simple low-pass + gain */
typedef struct AgOcclusion {
    float gain; /* 0..1 */
    float lpf_fc; /* Hz, 0 = no filter */
} AgOcclusion;

AgOcclusion ag_3d_occlusion(float obstruction); /* obstruction 0..1 */

/* Full spatializer - stateful for filters and delay for ITD */
typedef struct AgSpatializer {
    AgBiquad air_lpf_l, air_lpf_r;
    AgBiquad occ_lpf_l, occ_lpf_r;
    float itd_buf_l[1024];
    float itd_buf_r[1024];
    int itd_pos;
    int sr;
    AgOcclusion occ;
} AgSpatializer;

void ag_spatializer_init(AgSpatializer *spat, int sr);
void ag_spatializer_set_occlusion(AgSpatializer *spat, float obstruction);
void ag_spatializer_process(AgSpatializer *spat, const AgSource *src, const AgListener *lis, float in_mono, float *out_l, float *out_r);

/* Reverb zone */
typedef struct AgReverbZone {
    AgVec3 pos;
    float radius;
    float reverb_gain; /* 0..1 how much reverb */
    float damping;
    float room_size;
} AgReverbZone;

float ag_3d_reverb_zone_gain(const AgReverbZone *zone, AgVec3 listener_pos);

/* 3D Ambience bed - multiple point sources + ambient */
#define AG_3D_MAX_SOURCES 32

typedef struct Ag3DMixer {
    AgListener listener;
    AgSource sources[AG_3D_MAX_SOURCES];
    AgSpatializer spatializers[AG_3D_MAX_SOURCES];
    float source_gains[AG_3D_MAX_SOURCES];
    int source_active[AG_3D_MAX_SOURCES];
    int source_count;
    int sr;
    float master_gain;
} Ag3DMixer;

void ag_3d_mixer_init(Ag3DMixer *mix, int sr, AgVec3 listener_pos);
int ag_3d_mixer_add_source(Ag3DMixer *mix, AgVec3 pos, float min_dist, float max_dist);
void ag_3d_mixer_set_source_pos(Ag3DMixer *mix, int idx, AgVec3 pos);
void ag_3d_mixer_set_listener(Ag3DMixer *mix, AgVec3 pos, AgVec3 forward, AgVec3 up);
float ag_3d_mixer_render(Ag3DMixer *mix, float *in_mono_per_source, float *out_stereo_interleaved, int frames); /* in_mono_per_source is [source_count][frames] or null for silence, out is stereo */

#ifdef __cplusplus
}
#endif

#endif
