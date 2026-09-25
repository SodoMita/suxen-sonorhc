#include "ag_3d.h"
#include <string.h>
#include <math.h>

void ag_listener_init(AgListener *lis, AgVec3 pos) {
    memset(lis,0,sizeof(*lis));
    lis->pos=pos;
    lis->vel=ag_vec3(0,0,0);
    lis->forward=ag_vec3(0,0,-1);
    lis->up=ag_vec3(0,1,0);
    lis->right=ag_vec3(1,0,0);
    lis->speed_of_sound=343.0f;
    lis->doppler_factor=1.0f;
    lis->air_absorption=0.2f;
}
void ag_listener_set_orientation(AgListener *lis, AgVec3 forward, AgVec3 up) {
    lis->forward=ag_vec3_norm(forward);
    lis->up=ag_vec3_norm(up);
    lis->right=ag_vec3_norm(ag_vec3_cross(lis->forward, lis->up));
    /* re-orthogonalize up */
    lis->up=ag_vec3_norm(ag_vec3_cross(lis->right, lis->forward));
}
void ag_listener_update(AgListener *lis, AgVec3 pos, AgVec3 vel) {
    lis->pos=pos; lis->vel=vel;
}

void ag_source_init(AgSource *src, AgVec3 pos) {
    memset(src,0,sizeof(*src));
    src->pos=pos;
    src->vel=ag_vec3(0,0,0);
    src->gain=1.0f;
    src->min_dist=1.0f;
    src->max_dist=50.0f;
    src->rolloff=1.0f;
    src->dist_model=AG_DIST_INVERSE;
    src->cone_inner_angle=360.0f;
    src->cone_outer_angle=360.0f;
    src->cone_outer_gain=0.0f;
    src->cone_dir=ag_vec3(0,0,-1);
    src->is_ambient=0;
}
void ag_source_set_dist(AgSource *src, float min_dist, float max_dist, float rolloff, AgDistModel model) {
    src->min_dist = min_dist>0.01f?min_dist:0.01f;
    src->max_dist = max_dist>src->min_dist?max_dist:src->min_dist+1.0f;
    src->rolloff = rolloff>0?rolloff:1.0f;
    src->dist_model=model;
}
void ag_source_set_cone(AgSource *src, AgVec3 dir, float inner_deg, float outer_deg, float outer_gain) {
    src->cone_dir=ag_vec3_norm(dir);
    src->cone_inner_angle=inner_deg;
    src->cone_outer_angle=outer_deg;
    src->cone_outer_gain=ag_clamp_f(outer_gain,0,1);
}

float ag_3d_attenuation(const AgSource *src, float distance) {
    if(src->dist_model==AG_DIST_NONE) return 1.0f;
    float min_d = src->min_dist;
    float max_d = src->max_dist;
    float roll = src->rolloff;
    if(distance <= min_d) return 1.0f;
    if(distance >= max_d) {
        if(src->dist_model==AG_DIST_LINEAR) return 0.0f;
        /* inverse/exp still have some tail */
    }
    float att=1.0f;
    switch(src->dist_model){
        case AG_DIST_LINEAR: {
            att = 1.0f - roll * (distance - min_d) / (max_d - min_d);
            if(att<0) att=0;
        } break;
        case AG_DIST_INVERSE: {
            att = min_d / (min_d + roll * (distance - min_d));
        } break;
        case AG_DIST_EXP: {
            att = powf(min_d / distance, roll);
        } break;
        default: att=1.0f; break;
    }
    return ag_clamp_f(att,0,1);
}
float ag_3d_cone_gain(const AgSource *src, AgVec3 listener_pos) {
    if(src->cone_inner_angle>=360.0f) return 1.0f;
    AgVec3 to_listener = ag_vec3_norm(ag_vec3_sub(listener_pos, src->pos));
    float dot = ag_vec3_dot(to_listener, src->cone_dir);
    float angle = acosf(ag_clamp_f(dot,-1,1)) * 180.0f / (float)AG_PI;
    if(angle <= src->cone_inner_angle*0.5f) return 1.0f;
    if(angle >= src->cone_outer_angle*0.5f) return src->cone_outer_gain;
    float t = (angle - src->cone_inner_angle*0.5f) / (src->cone_outer_angle*0.5f - src->cone_inner_angle*0.5f);
    return ag_lerp_f(1.0f, src->cone_outer_gain, t);
}
float ag_3d_doppler_pitch(const AgSource *src, const AgListener *lis) {
    if(lis->doppler_factor<=0.001f) return 1.0f;
    AgVec3 to_listener = ag_vec3_sub(lis->pos, src->pos);
    float dist = ag_vec3_len(to_listener);
    if(dist<0.01f) return 1.0f;
    AgVec3 dir = ag_vec3_mul(to_listener, 1.0f/dist);
    float vls = ag_vec3_dot(lis->vel, dir);
    float vss = ag_vec3_dot(src->vel, dir);
    float denom = lis->speed_of_sound + vss * lis->doppler_factor;
    float numer = lis->speed_of_sound + vls * lis->doppler_factor;
    if(fabsf(denom)<0.01f) denom=0.01f;
    float pitch = numer / denom;
    return ag_clamp_f(pitch, 0.25f, 4.0f);
}

AgPanning ag_3d_pan_stereo(const AgSource *src, const AgListener *lis) {
    AgPanning p={0.5f,0.5f,0,0,0};
    AgVec3 to_src = ag_vec3_sub(src->pos, lis->pos);
    float dist = ag_vec3_len(to_src);
    if(dist<0.001f){ p.l=0.707f; p.r=0.707f; p.pan=0; return p; }
    AgVec3 dir = ag_vec3_mul(to_src, 1.0f/dist);
    /* Project onto listener's horizontal plane */
    float right_dot = ag_vec3_dot(dir, lis->right);
    float forward_dot = ag_vec3_dot(dir, lis->forward);
    /* pan -1..1 based on right_dot, with forward affecting width */
    float pan = right_dot;
    /* If behind, reduce stereo width slightly */
    if(forward_dot < 0) pan *= 0.7f;
    pan = ag_clamp_f(pan, -1.0f, 1.0f);
    p.pan=pan;
    /* equal power */
    float angle = (pan*0.5f + 0.5f) * (float)AG_PI * 0.5f;
    p.l = cosf(angle);
    p.r = sinf(angle);
    /* ILD based on pan */
    p.ild_db = pan * 6.0f;
    /* ITD ~ 0.6ms max */
    p.itd_ms = pan * 0.3f;
    return p;
}
AgPanning ag_3d_pan_binaural(const AgSource *src, const AgListener *lis) {
    AgPanning p = ag_3d_pan_stereo(src,lis);
    /* Enhance ITD/ILD for binaural */
    p.itd_ms *= 1.2f;
    p.ild_db *= 1.5f;
    /* Elevation cue via simple filtering will be done in spatializer */
    return p;
}

float ag_3d_air_absorption_fc(float distance, float base_fc) {
    /* Air absorbs high freqs: fc = base * exp(-distance * 0.01) */
    float factor = expf(-distance * 0.008f);
    return base_fc * factor;
}
AgOcclusion ag_3d_occlusion(float obstruction) {
    AgOcclusion occ;
    obstruction=ag_clamp_f(obstruction,0,1);
    occ.gain = 1.0f - obstruction*0.8f;
    if(obstruction>0.01f) occ.lpf_fc = 4000.0f * (1.0f - obstruction*0.8f) + 200.0f;
    else occ.lpf_fc=0;
    return occ;
}

void ag_spatializer_init(AgSpatializer *spat, int sr) {
    memset(spat,0,sizeof(*spat));
    spat->sr=sr>0?sr:AG_SR_DEFAULT;
    ag_biquad_init(&spat->air_lpf_l);
    ag_biquad_init(&spat->air_lpf_r);
    ag_biquad_set(&spat->air_lpf_l, AG_FILTER_LP, 20000, 0.7f, 0, (float)sr);
    ag_biquad_set(&spat->air_lpf_r, AG_FILTER_LP, 20000, 0.7f, 0, (float)sr);
    ag_biquad_init(&spat->occ_lpf_l);
    ag_biquad_init(&spat->occ_lpf_r);
    spat->occ.gain=1.0f;
}
void ag_spatializer_set_occlusion(AgSpatializer *spat, float obstruction) {
    spat->occ = ag_3d_occlusion(obstruction);
    if(spat->occ.lpf_fc>0){
        ag_biquad_set(&spat->occ_lpf_l, AG_FILTER_LP, spat->occ.lpf_fc, 0.7f, 0, (float)spat->sr);
        ag_biquad_set(&spat->occ_lpf_r, AG_FILTER_LP, spat->occ.lpf_fc, 0.7f, 0, (float)spat->sr);
    }
}
void ag_spatializer_process(AgSpatializer *spat, const AgSource *src, const AgListener *lis, float in_mono, float *out_l, float *out_r) {
    float dist = ag_vec3_dist(src->pos, lis->pos);
    float att = ag_3d_attenuation(src, dist);
    float cone = ag_3d_cone_gain(src, lis->pos);
    float gain = src->gain * att * cone * spat->occ.gain;

    AgPanning pan = ag_3d_pan_binaural(src,lis);

    /* air absorption */
    float air_fc = ag_3d_air_absorption_fc(dist, 20000.0f);
    if(air_fc < 18000){
        ag_biquad_set(&spat->air_lpf_l, AG_FILTER_LP, air_fc, 0.7f, 0, (float)spat->sr);
        ag_biquad_set(&spat->air_lpf_r, AG_FILTER_LP, air_fc, 0.7f, 0, (float)spat->sr);
    }

    float l = in_mono * pan.l * gain;
    float r = in_mono * pan.r * gain;

    l = ag_biquad_process(&spat->air_lpf_l, l);
    r = ag_biquad_process(&spat->air_lpf_r, r);

    if(spat->occ.lpf_fc>0){
        l = ag_biquad_process(&spat->occ_lpf_l, l);
        r = ag_biquad_process(&spat->occ_lpf_r, r);
    }

    /* Simple ITD via delay buffer (very crude) */
    int itd_samples = (int)(fabsf(pan.itd_ms) * 0.001f * spat->sr);
    if(itd_samples>1023) itd_samples=1023;
    if(itd_samples>0){
        if(pan.pan < 0){
            /* source left: right delayed */
            spat->itd_buf_r[spat->itd_pos] = r;
            int read = spat->itd_pos - itd_samples;
            if(read<0) read+=1024;
            r = spat->itd_buf_r[read];
            spat->itd_buf_l[spat->itd_pos]=l;
        } else {
            spat->itd_buf_l[spat->itd_pos]=l;
            int read = spat->itd_pos - itd_samples;
            if(read<0) read+=1024;
            l = spat->itd_buf_l[read];
            spat->itd_buf_r[spat->itd_pos]=r;
        }
        spat->itd_pos = (spat->itd_pos+1)%1024;
    }

    *out_l = l;
    *out_r = r;
}

float ag_3d_reverb_zone_gain(const AgReverbZone *zone, AgVec3 listener_pos) {
    float d = ag_vec3_dist(zone->pos, listener_pos);
    if(d >= zone->radius) return 0.0f;
    return (1.0f - d/zone->radius) * zone->reverb_gain;
}

void ag_3d_mixer_init(Ag3DMixer *mix, int sr, AgVec3 listener_pos) {
    memset(mix,0,sizeof(*mix));
    mix->sr=sr>0?sr:AG_SR_DEFAULT;
    mix->master_gain=1.0f;
    ag_listener_init(&mix->listener, listener_pos);
    for(int i=0;i<AG_3D_MAX_SOURCES;i++) ag_spatializer_init(&mix->spatializers[i], sr);
}
int ag_3d_mixer_add_source(Ag3DMixer *mix, AgVec3 pos, float min_dist, float max_dist) {
    if(mix->source_count >= AG_3D_MAX_SOURCES) return -1;
    int idx = mix->source_count++;
    ag_source_init(&mix->sources[idx], pos);
    ag_source_set_dist(&mix->sources[idx], min_dist, max_dist, 1.0f, AG_DIST_INVERSE);
    mix->source_active[idx]=1;
    mix->source_gains[idx]=1.0f;
    return idx;
}
void ag_3d_mixer_set_source_pos(Ag3DMixer *mix, int idx, AgVec3 pos) {
    if(idx<0||idx>=mix->source_count) return;
    mix->sources[idx].pos=pos;
}
void ag_3d_mixer_set_listener(Ag3DMixer *mix, AgVec3 pos, AgVec3 forward, AgVec3 up) {
    ag_listener_update(&mix->listener, pos, ag_vec3(0,0,0));
    ag_listener_set_orientation(&mix->listener, forward, up);
}
float ag_3d_mixer_render(Ag3DMixer *mix, float *in_mono_per_source, float *out_stereo_interleaved, int frames) {
    /* in_mono_per_source is [source_count][frames] interleaved as [src0_frame0, src1_frame0, ...] or null
       For simplicity, we assume in_mono_per_source is [source_count * frames] with source major? Actually we need per source buffer.
       We'll assume it's contiguous per source: source0 has frames, then source1, etc. Or if null, silence.
       Out is stereo interleaved [L,R,L,R...]
    */
    float peak=0;
    for(int f=0; f<frames; f++){
        float mix_l=0, mix_r=0;
        for(int s=0; s<mix->source_count; s++){
            if(!mix->source_active[s]) continue;
            float in = 0.0f;
            if(in_mono_per_source){
                /* layout: [source][frame] */
                in = in_mono_per_source[s*frames + f];
            }
            float l,r;
            ag_spatializer_process(&mix->spatializers[s], &mix->sources[s], &mix->listener, in, &l, &r);
            mix_l += l * mix->source_gains[s];
            mix_r += r * mix->source_gains[s];
        }
        mix_l *= mix->master_gain;
        mix_r *= mix->master_gain;
        out_stereo_interleaved[f*2]=mix_l;
        out_stereo_interleaved[f*2+1]=mix_r;
        float a = fabsf(mix_l); if(a>peak) peak=a;
        a = fabsf(mix_r); if(a>peak) peak=a;
    }
    return peak;
}
