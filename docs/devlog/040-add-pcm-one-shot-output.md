# Add PCM one-shot output

## Goal

Promote the smallest contract demonstrated by the click renderer probe into
`m5-tone-output`: play a caller-owned mono signed 8-bit PCM buffer through the
same configured M5Unified channel used by tones.

The probe established that short PCM shapes remain distinct on the M5StickC
Plus2 buzzer, that a wood-style buffer produces a useful discrete click, and
that repeated and overlapping starts return successfully. It also showed that
extreme 10 ms replacement pressure can occupy enough loop time for a simple
scheduler to fall behind. The package therefore exposes one-shot playback but
makes no real-time or sample-accurate retriggering promise.

## Boundary

The new non-owning descriptor is deliberately specific:

```cpp
struct PcmS8Sample {
  const int8_t* data;
  size_t sampleCount;
  uint32_t sampleRateHz;
};
```

`S8` means signed 8-bit PCM. Samples are mono; playback is one-shot; and a new
tone or sample replaces current output on the package-owned virtual channel.
The caller must keep the buffer valid and unchanged until playback finishes or
is stopped.

`M5ToneOutputCore::playSample()` rejects uninitialized output, null data, zero
samples, and a zero sample rate. Otherwise it forwards the fixed buffer to
M5Unified without allocation, filesystem access, decoding, or copying at the
package boundary.

This API remains on the concrete M5 output package. `VoiceOutput` stays focused
on note-oriented instruments, and the package does not acquire metronome,
envelope, sample-generation, or musical-sound policy. The existing package is
extended rather than creating a second package with duplicate M5 speaker
configuration and channel ownership.

Finite samples also revealed that the previous cached `isPlaying()` flag was
insufficient: playback can finish without an explicit `stop()`. The method now
queries the actual M5Unified virtual channel.

## Verification

```sh
just check
pio run -d ci/consumers/m5-tone-output
```

The click renderer probe now calls `playSample()` rather than reaching directly
into `M5.Speaker.playRaw()`. This preserves the already validated sound buffers
while exercising the proposed package boundary on hardware. The checked-in CI
consumer also compiles construction and playback of a `PcmS8Sample`.

## Hardware result

The migrated probe preserved the previously validated PCM output through
`M5ToneOutputCore::playSample()`. Noise and wood buffers remained audible; the
package boundary introduced no apparent change from direct `playRaw()` use.

After this validation, the temporary eight-trigger overlap burst was removed
from the probe. Button A again produces one click, leaving the app useful for
ordinary renderer comparison while the stress-test evidence remains recorded
in the preceding devlog chapter.

The additive package release is versioned as `m5-tone-output` `0.1.4`.
Consumers can adopt PCM playback explicitly while existing tone and
`VoiceOutput` consumers remain source compatible.
