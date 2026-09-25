#ifndef AG_ENVELOPE_H
#define AG_ENVELOPE_H

#include "ag_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AG_ENV_IDLE = 0,
    AG_ENV_ATTACK,
    AG_ENV_DECAY,
    AG_ENV_SUSTAIN,
    AG_ENV_RELEASE
} AgEnvStage;

typedef struct AgADSR {
    float attack;   /* sec */
    float decay;
    float sustain;  /* 0..1 level */
    float release;
    float attack_curve;  /* 1=lin, >1 exp, <1 log */
    float decay_curve;
    float release_curve;
} AgADSR;

typedef struct AgEnv {
    AgADSR adsr;
    AgEnvStage stage;
    double time;        /* time in current stage */
    double value;       /* current value 0..1 */
    int released;       /* has note-off been triggered? */
    double sr;
    double release_start_value;
} AgEnv;

/* Presets */
AgADSR ag_adsr_pluck(void);
AgADSR ag_adsr_pad(void);
AgADSR ag_adsr_bass(void);
AgADSR ag_adsr_stab(void);
AgADSR ag_adsr_perc(float decay);

void ag_env_init(AgEnv *env, AgADSR adsr, double sr);
void ag_env_trigger(AgEnv *env);
void ag_env_release(AgEnv *env);
int ag_env_is_active(const AgEnv *env);
int ag_env_is_idle(const AgEnv *env);
float ag_env_next(AgEnv *env);
float ag_env_value(const AgEnv *env);

/* AR envelope (attack-release) for one-shots */
typedef struct AgAR {
    float attack;
    float release;
    float curve;
} AgAR;

typedef struct AgEnvAR {
    AgAR ar;
    double t;
    double dur;
    int active;
    double sr;
} AgEnvAR;

void ag_env_ar_init(AgEnvAR *env, AgAR ar, double sr);
void ag_env_ar_trigger(AgEnvAR *env, double duration);
float ag_env_ar_next(AgEnvAR *env);

/* Multi-segment envelope */
#define AG_ENV_MAX_POINTS 16
typedef struct AgEnvPoint {
    float time;   /* sec from start */
    float value;
    float curve;  /* shaping */
} AgEnvPoint;

typedef struct AgMultiEnv {
    AgEnvPoint points[AG_ENV_MAX_POINTS];
    int count;
    double t;
    int active;
    int loop;
    double sr;
} AgMultiEnv;

void ag_multi_env_init(AgMultiEnv *env, double sr);
void ag_multi_env_add_point(AgMultiEnv *env, float time, float value, float curve);
void ag_multi_env_trigger(AgMultiEnv *env);
float ag_multi_env_next(AgMultiEnv *env);

#ifdef __cplusplus
}
#endif

#endif
