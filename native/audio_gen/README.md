# Audio Generators Library (C)

A collection of procedural music and sound generators in pure C, designed for games (Godot 4.7) and offline WAV rendering. Deterministic RNG, no malloc in audio thread (except reverb/delay init), portable (works on Android with `portable.c` fallback).

This extends the existing `SceneScore` live score mixer with many more generators for future use.

## Modules

- **ag_common**: RNG (xorshift64), scale helpers (major/minor/dorian/phrygian/lydian/mixolydian/pentatonic/blues/chromatic/octatonic/whole-tone), buffer utils, MIDI<->freq.
- **ag_osc**: Bandlimited oscillators (sine, saw, square, tri, noise, pulse) with polyBLEP, wavetable osc, LFO.
- **ag_envelope**: ADSR (with curve), AR, multi-segment envelope.
- **ag_filter**: Biquad (LP/HP/BP/notch/peak/shelf), one-pole LP/HP, SVF (simultaneous LP/HP/BP/notch), DC blocker, Moog ladder approx.
- **ag_noise**: White, pink (Kellet), brown, velvet (sparse impulses), crackle.
- **ag_sfx**: sfxr-like retro SFX synth. Params: wave, base_freq, ramp, vibrato, duty, ADSR+punch, filter, phaser, repeat, arp. Presets: coin, laser, explosion, powerup, hit, jump, blip, click, sweep, chime, buzz, whoosh, open, close.
- **ag_drums**: Kick (sine sweep + click), snare (tone+noise BP), hihat (noise+metallic squares), clap (multi-burst), toms, rim, cowbell, cymbal. Drum machine pattern (16 steps, 8 tracks, swing) + euclidean.
- **ag_fm**: 2-op FM (mod+car) and 4-op FM with algorithms (stack, 3+1, 2x2). Presets: bass, lead, pad, bell, epiano, brass.
- **ag_chiptune**: NES 2A03-ish: 2 squares (4 duties), triangle, noise (LFSR long/short). Chip mixer + arpeggiator + 16-step pattern sequencer.
- **ag_ambient**: Drone (3 detuned sines+LFO), wind (pink+LP+gust LFO), rain (random drops BP), granular pad (cloud of sine grains with pan), shimmer (octave-up delay), underwater (LP+HP+LFO).
- **ag_music_box**: Karplus-Strong plucked string, tine (sine+overtone), bell (8 inharmonic partials), kalimba (tine+wood), music box sequencer (16 voices, 128 notes).
- **ag_sequencer**: Step sequencer (64 steps, 8 tracks, scale degree mapping), euclidean rhythm, arpeggiator (up/down/up-down/random/converge), chord generator (major/minor/dim/aug/sus2/sus4/maj7/min7/dom7 + inversion).
- **ag_reverb**: Freeverb (8 combs + 4 allpass) with room_size/damping/wet/dry/width, plus cheaper Schroeder (4 combs+2 allpass).
- **ag_delay**: Delay line (feedback+LP filter), ping-pong stereo, tape delay (wow/flutter via LFO).
- **ag_wav**: WAV writer (float32, int16) and reader, plus mem writer.
- **ag_proc_music**: High-level procedural music. Mood-based spec (calm/warm/tense/night/dream/lofi/chiptune/ambient/rift/festival/lab), each with scale, chord progression, gains, plucks/bass per bar, shape, drums. Live scheduler like SceneScore: queue bars ahead, spawn voices, crossfade mixer of 2 layers, optional reverb/delay and drum machine.

## Build

```sh
cd native/audio_gen
./build.sh
```

- Host tests: `bin/test_all` (runs all unit checks)
- Sample generator: `bin/gen_samples` -> writes `/tmp/ag_*.wav`
- GDExtension: if `zig` is available, builds `addons/audio_gen/bin/libaudio_gen.*` for linux/windows/macos.

## Usage in C

```c
#include "audio_gen.h"

AgSfxParams sp;
ag_sfx_preset_coin(&sp);
float buf[44100];
ag_sfx_render(&sp, buf, 44100, 44100);
ag_wav_write_f32("coin.wav", buf, 44100, 1, 44100);

AgProcSpec spec;
ag_proc_spec_from_mood(&spec, AG_MOOD_LOFI, 60, 82, 12345);
spec.id=1;
AgProcMusic pm;
ag_proc_init(&pm, &spec, 44100);
float stereo[44100*2];
ag_proc_render(&pm, stereo, 44100);
```

## Godot Integration

A minimal GDExtension `AudioGen` (RefCounted) is provided:

- `render_sfx(type:int, frames:PackedVector2Array)` - type 0=coin,1=laser,2=explosion,3=powerup,4=hit,5=jump,6=blip
- `render_drum(type:int, frames:PackedVector2Array)` - 0=kick,1=snare,2=hihat closed, etc.
- `render_fm(preset:int, frames:PackedVector2Array, freq:float)`
- `render_proc(frames:PackedVector2Array)` - renders current proc mixer
- `transition(mood:int, root:float, bpm:float, fade:float)` - mood 0=calm,1=warm,2=tense,3=night,4=dream,5=lofi,6=chiptune,7=ambient,8=rift,9=festival,10=lab

Add `addons/audio_gen/audio_gen.gdextension` (create if not present) pointing to `res://addons/audio_gen/bin/libaudio_gen.*`.

For live music, similar to SceneScore:

```gdscript
var gen = AudioGen.new()
gen.transition(5, 60, 82, 0.8) # lofi
var buf = PackedVector2Array()
buf.resize(1024)
gen.render_proc(buf)
playback.push_buffer(buf)
```

## Future Ideas

- Wavetable morphing synth
- Physical modeling (bowed string, reed)
- Granular cloud with sample import
- Spectral freeze / FFT
- Multi-band compressor for mastering
- MIDI file import -> procedural variation
- Live coding REPL

## License

Same as project (see LICENSE.txt). Pure C, no external deps except libm.
