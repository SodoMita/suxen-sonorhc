#include "mix.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect(int cond, const char *message) {
	if (!cond) {
		fprintf(stderr, "FAIL %s\n", message);
		failures++;
	}
}

static void fill_nexus(double *params) {
	int i;
	memset(params, 0, sizeof(double) * SCENE_SCORE_PARAMS);
	params[SCORE_BPM] = 96.0;
	params[SCORE_ROOT] = 74.0;
	params[SCORE_SHAPE] = 0.0;
	params[SCORE_PAD] = 0.22;
	params[SCORE_PLUCK] = 0.46;
	params[SCORE_BASS] = 0.16;
	params[SCORE_PLUCKS] = 8.0;
	params[SCORE_BASS_HITS] = 1.0;
	params[SCORE_PLUCK_SHIFT] = 24.0;
	params[SCORE_BASS_SHIFT] = -24.0;
	params[SCORE_SCALE_N] = 7.0;
	params[SCORE_SCALE + 0] = 0;
	params[SCORE_SCALE + 1] = 2;
	params[SCORE_SCALE + 2] = 4;
	params[SCORE_SCALE + 3] = 6;
	params[SCORE_SCALE + 4] = 7;
	params[SCORE_SCALE + 5] = 9;
	params[SCORE_SCALE + 6] = 11;
	params[SCORE_CHORD_N] = 4.0;
	for (i = 0; i < 4; i++) {
		int base = SCORE_CHORDS + i * 5;
		params[base] = 3;
	}
	params[SCORE_CHORDS + 1] = 0;
	params[SCORE_CHORDS + 2] = 2;
	params[SCORE_CHORDS + 3] = 4;
	params[SCORE_CHORDS + 5 + 1] = 4;
	params[SCORE_CHORDS + 5 + 2] = 6;
	params[SCORE_CHORDS + 5 + 3] = 1;
	params[SCORE_CHORDS + 10 + 1] = 5;
	params[SCORE_CHORDS + 10 + 2] = 0;
	params[SCORE_CHORDS + 10 + 3] = 2;
	params[SCORE_CHORDS + 15 + 1] = 2;
	params[SCORE_CHORDS + 15 + 2] = 4;
	params[SCORE_CHORDS + 15 + 3] = 6;
}

static void fill_core(double *params) {
	int i;
	memset(params, 0, sizeof(double) * SCENE_SCORE_PARAMS);
	params[SCORE_BPM] = 72.0;
	params[SCORE_ROOT] = 36.0;
	params[SCORE_SHAPE] = 2.0;
	params[SCORE_PAD] = 0.3;
	params[SCORE_PLUCK] = 0.08;
	params[SCORE_BASS] = 0.66;
	params[SCORE_PLUCKS] = 1.0;
	params[SCORE_BASS_HITS] = 2.0;
	params[SCORE_PLUCK_SHIFT] = 12.0;
	params[SCORE_BASS_SHIFT] = -12.0;
	params[SCORE_SCALE_N] = 3.0;
	params[SCORE_SCALE + 0] = 0;
	params[SCORE_SCALE + 1] = 3;
	params[SCORE_SCALE + 2] = 7;
	params[SCORE_CHORD_N] = 4.0;
	for (i = 0; i < 4; i++) {
		params[SCORE_CHORDS + i * 5] = 3;
	}
	params[SCORE_CHORDS + 1] = 0;
	params[SCORE_CHORDS + 2] = 1;
	params[SCORE_CHORDS + 3] = 2;
	params[SCORE_CHORDS + 6] = 0;
	params[SCORE_CHORDS + 7] = 2;
	params[SCORE_CHORDS + 8] = 1;
}

static double rms(const float *samples, int frames) {
	double acc = 0.0;
	int i;
	for (i = 0; i < frames * 2; i++) {
		acc += (double)samples[i] * (double)samples[i];
	}
	return sqrt(acc / (double)(frames * 2));
}

static int differs(const float *a, const float *b, int n) {
	int i;
	for (i = 0; i < n; i++) {
		if (a[i] != b[i]) {
			return 1;
		}
	}
	return 0;
}

int main(void) {
	double params[SCENE_SCORE_PARAMS];
	ScoreSpec spec;
	ScoreSpec other;
	Mixer a;
	Mixer b;
	enum { N = SCENE_SCORE_RATE };
	static float buf_a[N * 2];
	static float buf_b[N * 2];
	static float buf_c[N * 2];
	double level;
	double next_before;

	fill_nexus(params);
	expect(score_from_params(&spec, params, SCENE_SCORE_PARAMS, 11, 20260921) == 0, "parse nexus");
	expect(spec.plucks == 8, "pluck count");
	expect(spec.chord_n == 4, "chord count");
	expect(spec.chord[1][0] == 4, "second chord");

	mixer_init(&a);
	mixer_init(&b);
	mixer_transition(&a, &spec, 0.05);
	mixer_transition(&b, &spec, 0.05);
	mixer_render(&a, buf_a, N);
	mixer_render(&b, buf_b, N);
	expect(!differs(buf_a, buf_b, N * 2), "same seed is deterministic");
	level = rms(buf_a + (N / 5) * 2, N - N / 5);
	fprintf(stderr, "rms %g notes %d\n", level, a.notes_scheduled);
	expect(level > 0.01, "live score is audible");
	expect(level < 1.2, "live score is not clipping hard");

	mixer_render(&a, buf_c, N);
	expect(differs(buf_a, buf_c, N * 2), "the next second is not a frozen loop");

	mixer_init(&a);
	mixer_init(&b);
	mixer_transition(&a, &spec, 0.05);
	mixer_transition(&b, &spec, 0.05);
	mixer_render(&a, buf_a, N / 4);
	mixer_render(&b, buf_b, N / 4);
	mixer_reseed(&b, 99);
	/* First bar is already queued. The next bar is a few seconds out. */
	{
		static float tail_a[N * 2];
		static float tail_b[N * 2];
		int hopped;
		for (hopped = 0; hopped < 4; hopped++) {
			mixer_render(&a, buf_a, N);
			mixer_render(&b, buf_b, N);
		}
		mixer_render(&a, tail_a, N);
		mixer_render(&b, tail_b, N);
		expect(differs(tail_a, tail_b, N * 2), "reseed changes notes that have not played yet");
	}

	fill_core(params);
	expect(score_from_params(&other, params, SCENE_SCORE_PARAMS, 22, 7) == 0, "parse core");
	mixer_init(&a);
	mixer_transition(&a, &spec, 0.05);
	mixer_render(&a, buf_a, N / 2);
	mixer_transition(&a, &other, 0.25);
	mixer_render(&a, buf_b, N / 2);
	level = rms(buf_b, N / 2);
	fprintf(stderr, "crossfade rms %g\n", level);
	expect(level > 0.008, "crossfade does not mute the handoff");
	expect(a.layers[a.primary].spec.id == 22, "incoming score is the new scene");
	expect(a.primary == 1, "crossfade uses the second live layer");

	mixer_init(&a);
	mixer_transition(&a, &spec, 0.05);
	mixer_render(&a, buf_a, 64);
	next_before = a.layers[a.primary].bar_len;
	other = spec;
	other.bpm = 180.0;
	mixer_adjust(&a, &other);
	expect(a.layers[a.primary].spec.bpm == 180.0, "adjust keeps the same layer");
	expect(a.layers[a.primary].bar_len < next_before * 0.7, "adjust shortens the next bar");
	expect(a.layers[a.primary].voice_n > 0, "adjust does not drop sounding voices");

	mixer_release(&a, 0.2);
	mixer_render(&a, buf_a, N);
	expect(a.master < 0.001, "release fades the master out");

	if (failures) {
		fprintf(stderr, "%d failure(s)\n", failures);
		return 1;
	}
	printf("scene score mixer ok\n");
	return 0;
}
