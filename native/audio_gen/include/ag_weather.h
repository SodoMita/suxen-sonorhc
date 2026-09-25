#ifndef AG_WEATHER_H
#define AG_WEATHER_H

#include "ag_common.h"
#include "ag_noise.h"
#include "ag_filter.h"
#include "ag_osc.h"
#include "ag_envelope.h"
#include "ag_water.h"
#include "ag_3d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AG_WEATHER_CLEAR = 0,
    AG_WEATHER_CLOUDY,
    AG_WEATHER_RAIN_LIGHT,
    AG_WEATHER_RAIN_MEDIUM,
    AG_WEATHER_RAIN_HEAVY,
    AG_WEATHER_THUNDERSTORM,
    AG_WEATHER_SNOW_LIGHT,
    AG_WEATHER_SNOW_HEAVY,
    AG_WEATHER_FOG,
    AG_WEATHER_WINDY,
    AG_WEATHER_HAIL,
    AG_WEATHER_COUNT
} AgWeatherType;

typedef struct AgRainSystem {
    AgNoise noise;
    AgBiquad lp;
    AgBiquad bp;
    AgRng rng;
    double sr;
    float gain;
    float density;
    float intensity; /* 0..1 */
    AgWeatherType type;
    double drip_timer;
} AgRainSystem;

void ag_rain_sys_init(AgRainSystem *rs, double sr);
void ag_rain_sys_set(AgRainSystem *rs, AgWeatherType type, float intensity);
float ag_rain_sys_next(AgRainSystem *rs);

typedef struct AgThunder {
    AgNoise noise;
    AgBiquad lp;
    AgBiquad bp;
    AgOsc rumble_lfo;
    AgRng rng;
    double sr;
    double timer;
    double next_thunder;
    float gain;
    int active;
    double active_time;
    double active_dur;
    AgEnv env;
} AgThunder;

void ag_thunder_init(AgThunder *th, double sr);
void ag_thunder_trigger(AgThunder *th);
float ag_thunder_next(AgThunder *th);
void ag_thunder_auto(AgThunder *th, float storm_intensity);

typedef struct AgWindSystem {
    AgNoise noise;
    AgBiquad lp1, lp2;
    AgOsc gust_lfo1, gust_lfo2;
    AgOsc turbulence_lfo;
    AgRng rng;
    double sr;
    float gain;
    float base_strength;
    float gust_strength;
    float turbulence;
} AgWindSystem;

void ag_wind_sys_init(AgWindSystem *ws, double sr);
void ag_wind_sys_set(AgWindSystem *ws, float base_strength, float gust_strength, float turbulence);
float ag_wind_sys_next(AgWindSystem *ws);

typedef struct AgWeatherMixer {
    AgRainSystem rain;
    AgThunder thunder;
    AgWindSystem wind;
    AgWeatherType type;
    float intensity;
    double sr;
    float gain;
} AgWeatherMixer;

void ag_weather_mixer_init(AgWeatherMixer *wm, double sr);
void ag_weather_mixer_set(AgWeatherMixer *wm, AgWeatherType type, float intensity);
float ag_weather_mixer_next(AgWeatherMixer *wm);
void ag_weather_mixer_next_stereo(AgWeatherMixer *wm, float *l, float *r);

#ifdef __cplusplus
}
#endif

#endif
