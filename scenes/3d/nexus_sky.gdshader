shader_type spatial;
render_mode unshaded, cull_disabled, depth_draw_never;

uniform float uTime = 0.0;
uniform float uVignette = 1.0;
uniform float uBloom = 1.0;
uniform vec4 uColorA : source_color = vec4(0.05, 0.02, 0.15, 1.0);
uniform vec4 uColorB : source_color = vec4(0.4, 0.1, 0.6, 1.0);
uniform vec4 uColorC : source_color = vec4(0.1, 0.4, 0.8, 1.0);
uniform vec4 uColorCore : source_color = vec4(1.0, 0.8, 0.5, 1.0);

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	return mix(
		mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x),
		mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x),
		f.y);
}

float fbm(vec2 p) {
	float v = 0.0;
	float a = 0.5;
	for (int i = 0; i < 5; i++) {
		v += a * noise(p);
		p *= 2.0;
		a *= 0.5;
	}
	return v;
}

void vertex() {
	// Use the local position as the direction.
}

void fragment() {
	vec3 dir = normalize(VERTEX);
	vec2 uv = dir.xy * 0.5 + 0.5;
	float t = uTime * 0.15;

	// Background void.
	vec3 col = mix(uColorA.rgb, uColorB.rgb, dir.y * 0.5 + 0.5);
	col += vec3(0.1, 0.05, 0.2) * fbm(uv * 4.0 + vec2(t * 0.3, t * 0.2)) * 0.4;

	// Concentric rings at the center.
	vec2 p = dir.xz;
	if (length(p) > 0.001) {
		float r = length(p);
		for (int i = 1; i <= 5; i++) {
			float fi = float(i);
			float ringR = fi * 0.15;
			float ring = exp(-pow((r - ringR) * 12.0, 2.0));
			vec3 ringCol = mix(uColorB.rgb, uColorC.rgb, fract(fi * 0.37));
			col += ringCol * ring * 0.5;
		}
	}

	// Central glowing core.
	float coreDist = length(dir - vec3(0.0, 0.0, -1.0));
	col += uColorCore.rgb * exp(-coreDist * 6.0) * 1.5;
	col += uColorCore.rgb * exp(-coreDist * 14.0) * 2.0;

	// Floating particles.
	for (int i = 0; i < 20; i++) {
		float fi = float(i);
		vec3 p2 = vec3(
			sin(fi * 7.3 + t) * 2.0,
			cos(fi * 5.7) * 1.5,
			sin(fi * 9.1 + t * 0.7) * 2.0 - 1.0);
		float d = length(dir - normalize(p2));
		col += vec3(0.7, 0.85, 1.0) * smoothstep(0.04, 0.0, d) * (0.7 + 0.3 * sin(t * 4.0 + fi));
	}

	// Vignette.
	if (uVignette > 0.5) {
		float v = smoothstep(1.2, 0.4, length(uv - 0.5));
		col *= mix(0.5, 1.0, v);
	}

	// Bloom-like bright-spot boost.
	if (uBloom > 0.5) {
		float lum = dot(col, vec3(0.299, 0.587, 0.114));
		col += col * smoothstep(0.5, 1.0, lum) * 0.6;
	}

	ALBEDO = col;
}
