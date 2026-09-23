#include "mix.h"

#include <math.h>
#include <string.h>

#define TAU 6.28318530717958647692
#define LOOKAHEAD 0.6
#define BLOCK 256
#define MUSIC_GAIN 0.5

static double clampl(double v, double lo, double hi) {
	if (v < lo) {
		return lo;
	}
	if (v > hi) {
		return hi;
	}
	return v;
}

static int trunc_i(double v) {
	return (int)v;
}

static double bar_length(double bpm) {
	return 60.0 / clampl(bpm, 1.0, 400.0) * 4.0;
}

static void set_ramp(double *step, double current, double target, double fade_sec) {
	double dist = fabs(target - current);
	if (fade_sec <= 0.0001) {
		*step = dist > 0.0 ? dist : 1.0;
		return;
	}
	*step = dist / (fade_sec * (double)SCENE_SCORE_RATE);
	if (*step < 1e-12) {
		*step = 1e-12;
	}
}

static void ramp_toward(double *value, double target, double step) {
	if (*value < target) {
		*value += step;
		if (*value > target) {
			*value = target;
		}
	} else if (*value > target) {
		*value -= step;
		if (*value < target) {
			*value = target;
		}
	}
}

static double rand_range(uint64_t *state, double lo, double hi) {
	uint64_t x = *state;
	if (x == 0) {
		x = 0x9E3779B97F4A7C15ULL;
	}
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	*state = x;
	double unit = (double)(x >> 11) * (1.0 / 9007199254740992.0);
	return lo + (hi - lo) * unit;
}

static int degree_midi(int deg, int root, const int *scale, int n) {
	int oct;
	int idx;
	if (n <= 0) {
		return root;
	}
	if (deg >= 0) {
		oct = deg / n;
		idx = deg % n;
	} else {
		int neg = -deg;
		oct = -((neg + n - 1) / n);
		idx = deg % n;
		if (idx < 0) {
			idx += n;
		}
	}
	return root + 12 * oct + scale[idx];
}

void mixer_init(Mixer *mixer) {
	memset(mixer, 0, sizeof(*mixer));
	mixer->primary = -1;
}

int score_from_params(ScoreSpec *spec, const double *params, int count, uint64_t id, uint64_t seed) {
	double padded[SCENE_SCORE_PARAMS];
	int i;
	if (spec == 0 || (params == 0 && count > 0)) {
		return -1;
	}
	memset(padded, 0, sizeof(padded));
	padded[SCORE_BPM] = 72.0;
	padded[SCORE_ROOT] = 60.0;
	padded[SCORE_PAD] = 0.4;
	padded[SCORE_PLUCK] = 0.2;
	padded[SCORE_BASS] = 0.3;
	padded[SCORE_PLUCKS] = 4.0;
	padded[SCORE_BASS_HITS] = 1.0;
	padded[SCORE_PLUCK_SHIFT] = 12.0;
	padded[SCORE_BASS_SHIFT] = -12.0;
	padded[SCORE_SCALE_N] = 5.0;
	padded[SCORE_SCALE + 0] = 0.0;
	padded[SCORE_SCALE + 1] = 2.0;
	padded[SCORE_SCALE + 2] = 4.0;
	padded[SCORE_SCALE + 3] = 7.0;
	padded[SCORE_SCALE + 4] = 9.0;
	padded[SCORE_CHORD_N] = 1.0;
	padded[SCORE_CHORDS] = 3.0;
	padded[SCORE_CHORDS + 1] = 0.0;
	padded[SCORE_CHORDS + 2] = 2.0;
	padded[SCORE_CHORDS + 3] = 4.0;
	if (count > SCENE_SCORE_PARAMS) {
		count = SCENE_SCORE_PARAMS;
	}
	for (i = 0; i < count; i++) {
		padded[i] = params[i];
	}
	memset(spec, 0, sizeof(*spec));
	spec->bpm = clampl(padded[SCORE_BPM], 1.0, 400.0);
	spec->root = trunc_i(padded[SCORE_ROOT]);
	spec->shape = padded[SCORE_SHAPE];
	spec->pad = clampl(padded[SCORE_PAD], 0.0, 1.5);
	spec->pluck = clampl(padded[SCORE_PLUCK], 0.0, 1.5);
	spec->bass = clampl(padded[SCORE_BASS], 0.0, 1.5);
	spec->plucks = trunc_i(padded[SCORE_PLUCKS]);
	if (spec->plucks < 0) {
		spec->plucks = 0;
	}
	if (spec->plucks > 16) {
		spec->plucks = 16;
	}
	spec->bass_hits = trunc_i(padded[SCORE_BASS_HITS]);
	if (spec->bass_hits < 0) {
		spec->bass_hits = 0;
	}
	if (spec->bass_hits > 4) {
		spec->bass_hits = 4;
	}
	spec->pluck_shift = trunc_i(padded[SCORE_PLUCK_SHIFT]);
	spec->bass_shift = trunc_i(padded[SCORE_BASS_SHIFT]);
	spec->scale_n = trunc_i(padded[SCORE_SCALE_N]);
	if (spec->scale_n < 0) {
		spec->scale_n = 0;
	}
	if (spec->scale_n > SCENE_SCORE_SCALE_MAX) {
		spec->scale_n = SCENE_SCORE_SCALE_MAX;
	}
	for (i = 0; i < spec->scale_n; i++) {
		spec->scale[i] = trunc_i(padded[SCORE_SCALE + i]);
	}
	spec->chord_n = trunc_i(padded[SCORE_CHORD_N]);
	if (spec->chord_n < 0) {
		spec->chord_n = 0;
	}
	if (spec->chord_n > SCENE_SCORE_CHORD_MAX) {
		spec->chord_n = SCENE_SCORE_CHORD_MAX;
	}
	for (i = 0; i < spec->chord_n; i++) {
		int base = SCORE_CHORDS + i * (SCENE_SCORE_TONE_MAX + 1);
		int k;
		int size = trunc_i(padded[base]);
		if (size < 0) {
			size = 0;
		}
		if (size > SCENE_SCORE_TONE_MAX) {
			size = SCENE_SCORE_TONE_MAX;
		}
		spec->chord_size[i] = size;
		for (k = 0; k < size; k++) {
			spec->chord[i][k] = trunc_i(padded[base + 1 + k]);
		}
	}
	spec->id = id;
	spec->seed = seed == 0 ? 1 : seed;
	return 0;
}

static void queue_add(Layer *layer, Event event) {
	int i;
	if (layer->queue_n >= SCENE_SCORE_QUEUE) {
		return;
	}
	i = layer->queue_n;
	while (i > 0 && layer->queue[i - 1].t > event.t) {
		layer->queue[i] = layer->queue[i - 1];
		i--;
	}
	layer->queue[i] = event;
	layer->queue_n++;
}

static void add_voice(Layer *layer, int midi, int kind, double dur, double peak, double pan, double detune) {
	Voice *voice;
	double freq;
	double ang;
	if (layer->voice_n >= SCENE_SCORE_VOICES || dur <= 0.0) {
		return;
	}
	freq = 440.0 * pow(2.0, ((double)midi - 69.0) / 12.0) * (1.0 + detune);
	ang = TAU * freq / (double)SCENE_SCORE_RATE;
	voice = &layer->voices[layer->voice_n++];
	memset(voice, 0, sizeof(*voice));
	voice->px = 1.0;
	voice->dx = cos(ang);
	voice->dy = sin(ang);
	voice->dur = dur;
	voice->peak = peak;
	voice->kind = kind;
	if (kind == 1) {
		voice->atk = 0.004;
		voice->rel = 0.05;
		voice->tau = dur / 4.5;
	} else if (kind == 2) {
		voice->atk = 0.02;
		voice->rel = 0.18;
		voice->tau = 1.0;
	} else {
		voice->atk = dur * 0.35;
		if (voice->atk > 0.6) {
			voice->atk = 0.6;
		}
		voice->rel = dur * 0.4;
		if (voice->rel > 0.8) {
			voice->rel = 0.8;
		}
		voice->tau = 1.0;
	}
	if (voice->tau < 0.0001) {
		voice->tau = 0.0001;
	}
	voice->gl = 1.0 - 0.6 * (pan > 0.0 ? pan : 0.0);
	voice->gr = 1.0 - 0.6 * (pan < 0.0 ? -pan : 0.0);
}

static void spawn_event(Layer *layer, const Event *event) {
	if (event->kind == 0) {
		add_voice(layer, event->midi, 0, event->dur, event->peak * 0.5, event->pan, -0.0022);
		add_voice(layer, event->midi, 0, event->dur, event->peak * 0.5, -event->pan, 0.0022);
	} else {
		add_voice(layer, event->midi, event->kind, event->dur, event->peak, event->pan, 0.0);
	}
}

static void schedule_bar(Layer *layer, Mixer *mixer) {
	const ScoreSpec *spec = &layer->spec;
	double bar_len = layer->bar_len > 0.0 ? layer->bar_len : bar_length(spec->bpm);
	double bt = layer->next_bar;
	int bar = layer->bar_index;
	int chord_i;
	int i;
	const int *chord;
	int chord_n;
	if (spec->chord_n <= 0 || spec->scale_n <= 0) {
		layer->next_bar = bt + bar_len;
		layer->bar_index++;
		return;
	}
	chord_i = bar % spec->chord_n;
	if (chord_i < 0) {
		chord_i += spec->chord_n;
	}
	chord = spec->chord[chord_i];
	chord_n = spec->chord_size[chord_i];
	for (i = 0; i < chord_n; i++) {
		Event event;
		event.t = bt;
		event.midi = degree_midi(chord[i] + 7, spec->root, spec->scale, spec->scale_n);
		event.kind = 0;
		event.dur = bar_len * 1.15;
		event.peak = chord_n > 0 ? spec->pad / (double)chord_n : 0.0;
		event.pan = (i % 2 == 1) ? 0.22 : -0.22;
		queue_add(layer, event);
		layer->notes++;
		mixer->notes_scheduled++;
	}
	for (i = 0; i < spec->bass_hits; i++) {
		Event event;
		int root_deg = chord_n > 0 ? chord[0] : 0;
		event.t = bt + bar_len * 0.5 * (double)i;
		event.midi = degree_midi(root_deg, spec->root, spec->scale, spec->scale_n) + spec->bass_shift;
		event.kind = 2;
		event.dur = bar_len * 0.45;
		event.peak = spec->bass;
		event.pan = 0.0;
		queue_add(layer, event);
		layer->notes++;
		mixer->notes_scheduled++;
	}
	for (i = 0; i < spec->plucks; i++) {
		Event event;
		int deg = 0;
		event.t = bt + bar_len * ((double)i / (double)spec->plucks);
		if (chord_n > 0) {
			int idx = (i + bar) % chord_n;
			if (idx < 0) {
				idx += chord_n;
			}
			deg = chord[idx];
		}
		event.midi = degree_midi(deg, spec->root, spec->scale, spec->scale_n) + spec->pluck_shift;
		if (spec->plucks >= 8 && i % 3 == 2) {
			event.midi += 12;
		}
		event.kind = 1;
		event.dur = bar_len * 0.6;
		event.peak = spec->pluck;
		event.pan = rand_range(&layer->rng, -0.35, 0.35);
		queue_add(layer, event);
		layer->notes++;
		mixer->notes_scheduled++;
	}
	layer->next_bar = bt + bar_len;
	layer->bar_index++;
}

static void reset_layer(Layer *layer, const ScoreSpec *spec, double fade_sec) {
	memset(layer, 0, sizeof(*layer));
	layer->active = 1;
	layer->spec = *spec;
	layer->rng = spec->seed == 0 ? 1 : spec->seed;
	layer->bar_len = bar_length(spec->bpm);
	layer->gain = 0.0;
	layer->gain_target = 1.0;
	set_ramp(&layer->gain_step, 0.0, 1.0, fade_sec);
}

void mixer_transition(Mixer *mixer, const ScoreSpec *spec, double fade_sec) {
	int slot;
	if (mixer == 0 || spec == 0) {
		return;
	}
	if (fade_sec < 0.0) {
		fade_sec = 0.8;
	}
	if (mixer->primary >= 0) {
		Layer *current = &mixer->layers[mixer->primary];
		if (current->active && !current->retiring && current->spec.id == spec->id) {
			return;
		}
	}
	slot = 0;
	if (mixer->primary >= 0) {
		Layer *outgoing = &mixer->layers[mixer->primary];
		slot = 1 - mixer->primary;
		outgoing->retiring = 1;
		outgoing->gain_target = 0.0;
		set_ramp(&outgoing->gain_step, outgoing->gain, 0.0, fade_sec);
	}
	reset_layer(&mixer->layers[slot], spec, fade_sec);
	mixer->primary = slot;
	mixer->master_target = MUSIC_GAIN;
	set_ramp(&mixer->master_step, mixer->master, MUSIC_GAIN, mixer->master < 0.001 ? 0.35 : fade_sec);
}

void mixer_adjust(Mixer *mixer, const ScoreSpec *spec) {
	Layer *layer;
	double new_len;
	uint64_t id;
	uint64_t seed;
	if (mixer == 0 || spec == 0) {
		return;
	}
	if (mixer->primary < 0 || !mixer->layers[mixer->primary].active || mixer->layers[mixer->primary].retiring) {
		mixer_transition(mixer, spec, 0.35);
		return;
	}
	layer = &mixer->layers[mixer->primary];
	new_len = bar_length(spec->bpm);
	if (layer->next_bar > layer->playhead && layer->bar_len > 0.0) {
		double start = layer->next_bar - layer->bar_len;
		if (start < layer->playhead) {
			start = layer->playhead;
		}
		layer->next_bar = start + new_len;
	}
	id = layer->spec.id;
	seed = layer->spec.seed;
	layer->spec = *spec;
	layer->spec.id = id;
	layer->spec.seed = seed;
	layer->bar_len = new_len;
	layer->rng = layer->rng == 0 ? seed : layer->rng;
}

void mixer_reseed(Mixer *mixer, uint64_t seed) {
	if (mixer == 0 || mixer->primary < 0) {
		return;
	}
	if (seed == 0) {
		seed = 1;
	}
	mixer->layers[mixer->primary].rng = seed;
	mixer->layers[mixer->primary].spec.seed = seed;
}

void mixer_release(Mixer *mixer, double fade_sec) {
	int i;
	if (mixer == 0) {
		return;
	}
	if (fade_sec < 0.0) {
		fade_sec = 0.6;
	}
	for (i = 0; i < SCENE_SCORE_LAYERS; i++) {
		if (mixer->layers[i].active) {
			mixer->layers[i].retiring = 1;
			mixer->layers[i].gain_target = 0.0;
			set_ramp(&mixer->layers[i].gain_step, mixer->layers[i].gain, 0.0, fade_sec);
		}
	}
	mixer->master_target = 0.0;
	set_ramp(&mixer->master_step, mixer->master, 0.0, fade_sec);
}

void mixer_set_gain(Mixer *mixer, double gain, double fade_sec) {
	if (mixer == 0) {
		return;
	}
	mixer->master_target = clampl(gain, 0.0, 1.5);
	set_ramp(&mixer->master_step, mixer->master, mixer->master_target, fade_sec);
}

int mixer_active(const Mixer *mixer) {
	int i;
	if (mixer == 0) {
		return 0;
	}
	if (mixer->master > 0.0005 || mixer->master_target > 0.0005) {
		return 1;
	}
	for (i = 0; i < SCENE_SCORE_LAYERS; i++) {
		if (mixer->layers[i].active && (mixer->layers[i].gain > 0.0005 || mixer->layers[i].voice_n > 0)) {
			return 1;
		}
	}
	return 0;
}

static void spawn_due(Layer *layer, double block_end) {
	while (layer->queue_n > 0 && layer->queue[0].t <= block_end) {
		Event event = layer->queue[0];
		memmove(&layer->queue[0], &layer->queue[1], (size_t)(layer->queue_n - 1) * sizeof(Event));
		layer->queue_n--;
		spawn_event(layer, &event);
	}
}

static void mix_layer_sample(Layer *layer, double *left, double *right, double inv_sr) {
	int vi = 0;
	double shape = layer->spec.shape;
	while (vi < layer->voice_n) {
		Voice *voice = &layer->voices[vi];
		double env;
		double raw;
		double sample;
		double px;
		double py;
		if (voice->kind == 1) {
			env = exp(-voice->t / voice->tau);
			if (voice->t < voice->atk && voice->atk > 0.0) {
				env *= voice->t / voice->atk;
			}
		} else if (voice->t < voice->atk && voice->atk > 0.0) {
			env = voice->t / voice->atk;
		} else if (voice->t < voice->dur - voice->rel) {
			env = 1.0;
		} else if (voice->rel > 0.0) {
			env = (voice->dur - voice->t) / voice->rel;
			if (env < 0.0) {
				env = 0.0;
			}
		} else {
			env = 0.0;
		}
		if (env <= 0.0002 && voice->t > voice->atk) {
			layer->voice_n--;
			if (vi != layer->voice_n) {
				layer->voices[vi] = layer->voices[layer->voice_n];
			}
			continue;
		}
		raw = voice->py;
		if (shape >= 2.0) {
			double sign = raw > 0.0 ? 1.0 : (raw < 0.0 ? -1.0 : 0.0);
			raw = sign * 0.72 + raw * 0.28;
		} else if (shape >= 1.0) {
			raw = asin(clampl(raw, -1.0, 1.0)) * 0.63662;
		}
		sample = raw * env * voice->peak;
		*left += sample * voice->gl;
		*right += sample * voice->gr;
		px = voice->px;
		py = voice->py;
		voice->px = px * voice->dx - py * voice->dy;
		voice->py = px * voice->dy + py * voice->dx;
		voice->t += inv_sr;
		vi++;
	}
	ramp_toward(&layer->gain, layer->gain_target, layer->gain_step);
	if (layer->retiring && layer->gain <= 0.0005 && layer->voice_n == 0 && layer->queue_n == 0) {
		layer->active = 0;
	}
}

void mixer_render(Mixer *mixer, float *interleaved, int frames) {
	int pos = 0;
	double inv_sr = 1.0 / (double)SCENE_SCORE_RATE;
	if (mixer == 0 || interleaved == 0 || frames <= 0) {
		return;
	}
	while (pos < frames) {
		int block = BLOCK;
		int i;
		double block_end;
		int layer_i;
		if (block > frames - pos) {
			block = frames - pos;
		}
		block_end = mixer->layers[0].playhead;
		for (layer_i = 0; layer_i < SCENE_SCORE_LAYERS; layer_i++) {
			Layer *layer = &mixer->layers[layer_i];
			double end;
			if (!layer->active) {
				continue;
			}
			end = layer->playhead + (double)block * inv_sr;
			if (!layer->retiring) {
				int guard = 0;
				while (layer->next_bar <= end + 1e-9 && guard < 32) {
					schedule_bar(layer, mixer);
					guard++;
				}
			}
			spawn_due(layer, end);
			if (end > block_end) {
				block_end = end;
			}
		}
		(void)block_end;
		for (i = 0; i < block; i++) {
			double left = 0.0;
			double right = 0.0;
			for (layer_i = 0; layer_i < SCENE_SCORE_LAYERS; layer_i++) {
				Layer *layer = &mixer->layers[layer_i];
				double ll = 0.0;
				double rr = 0.0;
				if (!layer->active) {
					continue;
				}
				mix_layer_sample(layer, &ll, &rr, inv_sr);
				left += ll * layer->gain;
				right += rr * layer->gain;
				layer->playhead += inv_sr;
			}
			left = left / (1.0 + fabs(left)) * 1.4;
			right = right / (1.0 + fabs(right)) * 1.4;
			ramp_toward(&mixer->master, mixer->master_target, mixer->master_step);
			left *= mixer->master;
			right *= mixer->master;
			if (!isfinite(left)) {
				left = 0.0;
			}
			if (!isfinite(right)) {
				right = 0.0;
			}
			interleaved[(pos + i) * 2] = (float)left;
			interleaved[(pos + i) * 2 + 1] = (float)right;
		}
		pos += block;
	}
}
