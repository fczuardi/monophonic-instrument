# m5-tone-output

PlatformIO package for M5Unified speaker-like `VoiceOutput` backends used by
the embedded music experiments.

Current public classes:

- `M5ToneOutputCore`: shared M5Unified tone playback, waveform, volume, and
  velocity-to-volume behavior, plus mono signed 8-bit PCM one-shots.
- `M5BuzzerToneOutput`: M5StickC Plus2 buzzer configuration backed by
  `M5ToneOutputCore`.
- `M5CoreGrayToneOutput`: M5Stack Core Gray internal speaker defaults backed by
  `M5ToneOutputCore`.

Starting with `0.1.3`, the PlatformIO Registry package declares its dependency
on `fcz2/monophonic-instrument`, so registry consumers can install
`m5-tone-output` directly:

```ini
lib_deps =
  fcz2/m5-tone-output@0.1.4
```

The package will also host regular M5 speaker output classes, starting with the
Core Gray speaker backend.

## PCM one-shots

`PcmS8Sample` describes a fixed mono signed 8-bit PCM buffer without owning or
copying it:

```cpp
const int8_t clickData[] = {0, 48, -32, 12, 0};
const PcmS8Sample click = {
    clickData,
    sizeof(clickData) / sizeof(clickData[0]),
    16000,
};

output.playSample(click);
```

`playSample()` uses the same M5Unified speaker configuration, volume, and
virtual channel as tone playback. A new tone or sample replaces current output
on that channel. The data pointer must remain valid and unchanged until the
sample finishes or `stop()` is called. Playback performs no allocation,
filesystem access, or format decoding.

This capability intentionally stays on the concrete M5 output layer. It is not
part of the note-oriented `VoiceOutput` contract, and the package does not own
musical click sounds or envelope policy.

`PcmS8Sample` and `playSample()` are available starting with `0.1.4`.

### PCM idle behavior

Version 0.1.4 starts finite PCM one-shots on demand and allows the M5Unified
speaker path to become idle afterward. M5StickC Plus2 metronome testing found
that restarting output after idle can contribute a hardware transition that is
not present in the PCM buffer and may dominate quiet sounds. The package now
owns an explicit silent keep-alive policy, replacing the metronome's direct
channel workaround:

```cpp
M5BuzzerToneOutput output(AudioIdlePolicy::KeepAlive);
const bool ready = output.begin();
```

`StopWhenIdle` remains available where power saving or the natural speaker
lifecycle is preferable. `setIdlePolicy()` can switch behavior after startup.
`stop()` stops the musical channel while preserving keep-alive; `end()` stops
both channels and releases the speaker backend. Both `begin()` and
`setIdlePolicy()` report whether the requested policy was established.
