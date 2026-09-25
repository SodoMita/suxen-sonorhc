#ifndef AG_SOUNDSCAPE_H
#define AG_SOUNDSCAPE_H

#include "ag_common.h"
#include "ag_ambience_3d.h"
#include "ag_biome.h"
#include "ag_proc_music.h"

#ifdef __cplusplus
extern "C" {
#endif

/* High-level Soundscape - evolves over time, combines biome + weather + music + 3D points
 * Designed for VN scenes: classroom, grove, shore, nexus, rift, etc.
 */

typedef struct AgSoundscapeParams {
    AgBiomeType biome;
    AgMood mood; /* for background music */
    AgWeatherType weather;
    float weather_intensity;
    float time_of_day; /* 0..1 */
    float music_gain;
    float ambience_gain;
    float master_gain;
    uint64_t seed;
    int use_3d; /* if true, use 3D ambience */
    int use_music; /* if true, include proc music */
} AgSoundscapeParams;

typedef struct AgSoundscape {
    AgSoundscapeParams params;
    AgAmbience3D ambience_3d;
    AgBiome biome; /* fallback if not 3D */
    AgProcMixer music_mixer;
    AgWeatherMixer weather;
    double sr;
    double time;
    float gain;
    AgRng rng;
} AgSoundscape;

void ag_soundscape_params_default(AgSoundscapeParams *p);
void ag_soundscape_params_for_scene(const char *scene_name, AgSoundscapeParams *p, uint64_t seed);

void ag_soundscape_init(AgSoundscape *ss, const AgSoundscapeParams *params, double sr);
void ag_soundscape_set_params(AgSoundscape *ss, const AgSoundscapeParams *params, float fade_sec);
void ag_soundscape_set_time_of_day(AgSoundscape *ss, float tod, float fade_sec);
void ag_soundscape_set_weather(AgSoundscape *ss, AgWeatherType weather, float intensity, float fade_sec);
void ag_soundscape_set_mood(AgSoundscape *ss, AgMood mood, float fade_sec);

void ag_soundscape_render(AgSoundscape *ss, float *stereo_interleaved, int frames);
float ag_soundscape_next(AgSoundscape *ss, float *l, float *r);

/* Presets for Chrono Nexus scenes */
void ag_soundscape_preset_classroom(AgSoundscape *ss);
void ag_soundscape_preset_grove(AgSoundscape *ss);
void ag_soundscape_preset_shore(AgSoundscape *ss);
void ag_soundscape_preset_nexus(AgSoundscape *ss);
void ag_soundscape_preset_rift(AgSoundscape *ss);
void ag_soundscape_preset_lab(AgSoundscape *ss);
void ag_soundscape_preset_festival(AgSoundscape *ss);

#ifdef __cplusplus
}
#endif

#endif
