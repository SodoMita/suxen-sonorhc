shader_type canvas_item;

// Glitch transition shader. Splits the screen into horizontal bands
// and flips them between the old and new background with a static /
// chromatic-aberration effect, used to visually mark "rift" cuts.

uniform float progress : hint_range(0.0, 1.0);
uniform sampler2D previous_background : source_color, hint_default_transparent;
uniform sampler2D next_background : source_color, hint_default_transparent;

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void fragment() {
	vec2 uv = UV;
	// 14 horizontal bands.
	float band = floor(uv.y * 14.0);
	float band_seed = hash(vec2(band, floor(progress * 12.0)));
	float band_jitter = (band_seed - 0.5) * 0.12 * (1.0 - progress);
	// Bands flip a little earlier or later.
	float band_progress = clamp(progress + band_jitter, 0.0, 1.0);
	// Discrete on/off per band: once a band switches it stays switched.
	float switched = step(band_progress, 0.5);
	// Add a glitchy chromatic offset for the bands that are switching.
	vec2 chroma = vec2(0.01, 0.0) * (1.0 - abs(band_progress - 0.5) * 2.0);
	vec4 a = texture(previous_background, uv + chroma);
	vec4 b = texture(next_background, uv - chroma);
	vec4 col = mix(a, b, switched);
	// Scanline-like static noise.
	float static_noise = hash(uv * 800.0 + floor(progress * 30.0)) * 0.06;
	col.rgb += static_noise * (1.0 - progress);
	COLOR = col;
}
