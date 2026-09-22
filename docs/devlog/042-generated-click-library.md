# Generated click library

## Goal

Build an app-local catalog of deterministic click signals designed around what
the M5StickC Plus2 buzzer can reproduce, then identify a useful regular/accent
pair by listening on hardware.

The converted-WAV probe showed that fine differences among natural percussion
recordings survive 16 kHz signed 8-bit conversion on headphones but collapse
through the buzzer. Earlier synthetic tests showed that broad structural
differences—tone, noise, and damped resonance—remain distinct. This slice
therefore explores signals of our own rather than a larger sample collection.

## Catalog

`GeneratedClickLibrary` prepares eight fixed mono signed 8-bit PCM buffers at
16 kHz during setup:

1. `low knock`: 260 Hz damped body, 60 ms;
2. `wood block`: 720 Hz damped body plus noisy strike, 45 ms;
3. `bright clave`: 1760 Hz fast damped body, 30 ms;
4. `rim blend`: 940/2310 Hz bodies plus noisy strike, 35 ms;
5. `dry impulse`: six sparse alternating impulses, 20 ms;
6. `noise tap`: rapidly damped deterministic noise, 25 ms;
7. `brush noise`: slower linear noise decay, 60 ms;
8. `down sweep`: 1900 to 400 Hz damped sweep, 45 ms.

A 294 Hz, 45 ms saw tone remains as sound 1 for reference. The initial eight
generated sounds occupied 7,680 bytes of fixed RAM. They
use deterministic generation, no allocation, and the validated
`PcmS8Sample`/`playSample()` path.

These are experimental signals, not yet a shared library contract. Keeping
them local lets hardware listening remove ineffective designs before we decide
whether reusable value lies in generator algorithms, fixed sample assets, or
only a chosen metronome pair.

## Verification

```sh
just build
just upload-click
just monitor-click
```

Button B cycles all nine sounds and Button A triggers one click. The display
shows index, name, and duration. Listening should identify which sounds are
meaningfully distinct, comfortable when repeated, and suitable as regular or
accented beats.

## Long-body, higher-gain comparison

The initial 20-60 ms catalog raised the question of whether all candidates were
too short relative to the approximately 170 ms recorded WAV collection. The
next hardware pass changes every generated buffer to 160 ms and raises the M5
speaker volume from 96 to 128, the highest level previously found useful before
higher settings introduced distortion.

Tone, noise, and sweep envelopes now decay across the full 160 ms window. The
former dry impulse becomes a 10 ms-spaced decaying impulse train so its extended
buffer contains audible material rather than trailing silence. Frequencies and
other sound identities remain unchanged, isolating the effect of longer bodies
and higher output gain as much as practical.

The eight 160 ms buffers occupy 20,480 bytes of fixed RAM.

## Hardware result

The longer, louder generated catalog revealed a promising initial metronome
pair:

- `low knock` for the repeating regular beat;
- `bright clave` for the downbeat accent.

Both are generated recipes rather than imported recordings, and their broad
frequency/structure contrast survives the Plus2 buzzer. They are now the
selected pair to carry into the metronome integration.

The wider catalog also suggests a future buzzer drum machine and a compact
BFXR-like recipe model could be viable. That larger design remains deferred.
The current evidence selects two concrete sounds; it does not yet reveal a
stable editable-synthesis contract worth promoting into a shared package.
