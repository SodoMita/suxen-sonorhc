# Audio Generators Catalog

This document catalogs all C generators in `native/audio_gen/` for future use in Chrono Nexus and other projects.

## 1. Oscillators (`ag_osc`)

Bandlimited oscillators using polyBLEP for anti-aliased saw/square.

- Types: SINE, SAW, SQUARE, TRI, NOISE, PULSE, SAW_TRI morph
- `AgOsc`: phase 0..1, freq, sr, pw, detune, RNG for noise
- `ag_osc_next()` = bandlimited, `ag_osc_next_raw()` = naive
- Wavetable: 1024 samples, morph between tables
- LFO: wrapper with depth/offset

Use for: any synth voice, modulation.

## 2. Envelopes (`ag_envelope`)

- ADSR with curve per stage (pow shaping)
- AR (attack-release) for one-shots
- Multi-segment: up to 16 points, each with time/value/curve, optional loop

Presets: pluck, pad, bass, stab, perc(decay)

## 3. Filters (`ag_filter`)

- Biquad: LP, HP, BP, Notch, Peak, LowShelf, HighShelf (RBJ cookbook)
- One-pole LP/HP (cheap)
- SVF: simultaneous LP/HP/BP/Notch, with ic1eq/ic2eq state
- DC blocker: y = x - x1 + 0.995*y1
- Ladder: 4-pole Moog approx with tanh saturation and resonance

## 4. Noise (`ag_noise`)

- White: uniform -1..1
- Pink: Paul Kellet 1-pole filter bank
- Brown: leaky integrator
- Velvet: sparse -1/0/1 impulses, density param = avg distance
- Crackle: occasional cubed white spikes

## 5. SFX Synth (`ag_sfx`)

sfxr-inspired but cleaner. One voice = osc + vibrato LFO + LP/HP + AR + punch + phaser.

Params:
- wave, base_freq, freq_ramp, freq_dramp, vib_strength/speed, duty/duty_ramp
- attack/sustain/punch/decay, filter_on/type/lpf_freq/ramp/resonance/hpf_freq/ramp
- phaser_offset/ramp, repeat_speed, arp_speed/mod, gain, seed

Presets:
- coin: square 800Hz +1200 ramp, short
- laser: saw 1200Hz -2800 ramp
- explosion: noise 200Hz + LP 1200 -2000 ramp
- powerup: sine 300 +1800 ramp + arp 12 semitones
- hit, jump, blip, click, sweep, chime, buzz, whoosh, open, close

Render: `ag_sfx_render(params, out_mono, max_frames, sr)` or stereo.

## 6. Drums (`ag_drums`)

Each drum = osc(s) + noise + envelopes + filter.

- Kick: sine 60Hz * tune, pitch env *6, click = sin(3000Hz)*exp(-800t)*snap, tanh punch
- Snare: tone 180Hz + noise BP 2000Hz, snap transient
- Hihat: white noise + 6 square oscs at 3k+ for metallic, HP 4000
- Clap: 3 bursts, BP 1200+ burst*200
- Tom: sine with pitch env
- Rim: sine 1200Hz + noise click
- Cowbell: two sines 540Hz and 810Hz
- Cymbal: HP noise

Drum machine: 8 tracks, 16 steps, velocity 0..127, swing 0..0.6, pattern kits.

Euclidean: Bjorklund algorithm for rhythm.

## 7. FM Synth (`ag_fm`)

- 2-op: mod -> car, mod_index depth, feedback on mod
- 4-op: 4 algorithms (stack, 3+1, 2x2, 1+3)
- Op: osc (sine), env, level, freq_mul, detune

Presets:
- Bass: 1x mult, index 2.5, short ADSR
- Lead: index 4.0
- Pad: mod 0.5x, index 1.5, long pad ADSR
- Bell: mod 3.5x, index 6.0, long decay
- EPIANO: index 2.0, decay 0.6/0.8
- Brass: attack 0.08, index 3.0

## 8. Chiptune (`ag_chiptune`)

NES 2A03-ish:

- Square: phase, duty 12.5/25/50/75%, volume, ADSR, sweep placeholder
- Tri: phase, linear counter placeholder
- Noise: 15-bit LFSR, long (bit1 ^ bit0) vs short (bit6 ^ bit0), freq controls shift rate
- Chip: 2 squares + tri + noise, master gain, soft clip mix
- Arp: holds up to 8 notes, modes up/down/up-down/random/converge, octave range
- Sequencer: 16-step pattern with sq1/sq2/tri midi (-1 rest, -2 note-off) + noise on + duty, bpm, step_dur

## 9. Ambient (`ag_ambient`)

- Drone: 3 sines detuned (0, +detune, -detune*0.5), LP filter, LFO 0.1Hz depth 0.3, tiny white noise
- Wind: white+pink -> LP 800 + LP 400, gust LFO 0.15Hz, one-pole smooth 2Hz for envelope
- Rain: random drops (density 0..1 controls prob 0.1* density), each drop = white noise BP 1500-6000Hz, gain 0.3-1.0, plus drizzle
- Granular pad: cloud of 32 grains, each grain = 2 cycles sine * Hanning window, random pan, grain rate 20Hz, spread detune, LP filter
- Shimmer: delay line 44100 samples (1 sec), feedback 0.6, LP 3000, osc 2x base_freq for pitch mod (simulates octave up)
- Underwater: LP 600 + LFO 200 mod, HP 20

## 10. Music Box (`ag_music_box`)

- Karplus-Strong: delay line up to 4096, initial noise, feedback 0.995, LP in loop, damping
- Tine: sine + overtone 4x, ADSR 0.001/0.8/0/0.2, LP 6* freq
- Bell: 8 partials ratios 1,2,2.4,3,4.2,5.4,6.8,8, amps decreasing, decays 1..0.2, env *= (1 - 0.001/decay)
- Kalimba: tine + wood (KS 0.5x freq, 0.3 damping) mix 0.7
- MusicBox: 16 voices, 128 notes, each note time/midi/vel, playhead, next_note index, looping, render

## 11. Sequencer (`ag_sequencer`)

- Step: midi -1 rest, vel, gate, prob
- Track: 64 steps, length, root_midi, scale, use_scale flag (midi as degree)
- Sequencer: 8 tracks, steps per bar, bpm, sr, playhead, step_dur = 60/bpm/(steps/4), swing, rng, playing flag
- Tick: advances playhead, returns 1 on new step, swing adds to odd steps
- Euclidean: bucket method
- Arpeggiator: held_notes 16, pos, dir, mode, timer, step_dur, octave_range, current_midi
- Chord: types major/minor/dim/aug/sus2/sus4/maj7/min7/dom7, inversion rotates tones +12

## 12. Reverb (`ag_reverb`)

- Freeverb: 8 combs (tunings 1116,1188,1277,1356,1422,1491,1557,1617 scaled by sr/44100), 4 allpass (556,441,341,225), feedback 0.28+0.7*room_size, damping 0.4* damping, wet/dry/width/gain
- Schroeder: 4 combs + 2 allpass, simpler

## 13. Delay (`ag_delay`)

- Delay: circular buffer size = sr*max_delay_ms/1000, delay_ms, feedback, wet/dry, optional LP filter
- PingPong: left delay + right delay 1.5x, cross_feedback
- Tape: delay + wow LFO 0.5Hz depth 0.002 + flutter LFO 6Hz depth 0.001 modulating delay time

## 14. Distortion (`ag_distortion`)

- Dist: drive 1..20 * tanh, mix, tone LP (one-pole placeholder)
- Bitcrush: bit_depth 1..16 steps = 2^bits, sample_rate_div 1..100, hold time = div/sr, quantize
- Wavefolder: threshold, gain, fold via fmod
- Compressor: threshold_db, ratio, attack_ms, release_ms, makeup_db, env follower with attack/release rates exp(-1/(ms*sr))

## 15. Sampler (`ag_sampler`)

- Sample: float* mono, frames, sr, owned flag
- Voice: sample ptr, pos double, inc = 2^(semi/12), gain, pan_l/r, active, loop start/end
- Next: linear interpolation, loop or stop
- Instrument: up to 16 zones, each sample + root/low/high midi + tune cents
- Poly: 16 voices, find zone by midi, pitch = midi - root + tune/100

## 16. Formant (`ag_formant`)

- Formant: freq, bw, gain
- Vowel: name + 3 formants (A: 800/80,1150/90,2900/120; E: 400/70,1700/80,2600/100; etc)
- Filter: 3 BP biquads, Q = freq/bw
- Voice: saw source + formant filter
- Morph: interpolate freq/bw/gain between two vowels

## 17. Presets (`ag_presets`)

High-level game audio:

- UI: click, hover, confirm, back, error, save, open, close
- Gameplay: footstep (surface 0 grass 120Hz,1 stone 300,2 wood 200,3 metal 400 BP), jump, land (noise 80Hz LP400), pickup (coin/powerup/key), hit soft/hard, explosion small/med/large, laser, whoosh, teleport (200+2000 ramp vib), heal, levelup
- Ambient: wind_gust LP600+400 ramp, rain_drop sine 2500-1200, thunder noise 40Hz LP200
- Music: for_scene (classroom/nexus/rift/grove/shore/core/festival/lab/sanctum/alley/lighthouse/lofi/chiptune) -> mood/root/bpm mapping, for_mood
- Drum patterns: basic (kick 0,4,8,12 snare 4,12 hihat even), lofi (swing 0.12, kick 0,7,10 snare 4,11,15 rim), techno (kick 4/4 hihat 2/2 open 14 clap 4,12), ambient (kick 0 cymbal 8)

## 18. Procedural Music (`ag_proc_music`)

High-level live score, similar to SceneScore but more genres.

- Spec: mood enum, bpm, root_midi, scale, progression[8] chords, prog_len, pad/pluck/bass/drums gains, plucks_per_bar, bass_hits_per_bar, pluck_shift/bass_shift, shape, seed, id
- from_mood: maps mood to scale/chords/gains/bpm (e.g., calm pentatonic major, tense minor dim, lofi dorian min7/dom7/maj7, ambient whole-tone, rift octatonic, etc)
- ProcMusic: spec, rng, playhead, next_bar, bar_len=60/bpm*4, bar_index, queue[256] sorted by time, voices[64], gain ramping, reverb/delay optional, drum machine if drums_gain>0
- schedule_bar: pads = chord tones +12, dur 1.15*bar, pan alternating; bass = root+shift at 0.5*bar intervals; plucks = chord[(i+bar)%count]+shift, octave up every 3rd if >=8 plucks, pan random
- Render: block 256, schedule ahead, spawn due, mix voices with shape (0 sine,1 asin warm,2 hard clip 0.72), saturation, gain ramp, reverb/delay, drums
- Mixer: 2 layers, primary index, master gain ramp, transition crossfades (retiring layer gain->0), adjust keeps id/seed but updates bpm and next_bar, reseed changes rng, active check

## 19. WAV (`ag_wav`)

- Write float32 (format 3) or int16 (format 1), header 44 bytes little-endian
- Mem writer: realloc buffer, same header
- Reader: info (channels/sr/frames/bits/is_float), read float32 or int16 -> float

## Usage Example

```c
AgProcSpec spec;
ag_proc_spec_from_mood(&spec, AG_MOOD_LOFI, 60, 82, 12345);
spec.id=1;
AgProcMixer mixer;
ag_proc_mixer_init(&mixer, 44100);
ag_proc_mixer_transition(&mixer, &spec, 0.8f);
float buf[1024*2];
ag_proc_mixer_render(&mixer, buf, 1024);
```

All generators are deterministic with seed, no malloc in audio thread after init, portable.
