# Click renderer probe

## Goal

Determine whether short PCM one-shots can produce a more natural M5StickC
Plus2 metronome click than the existing repeating tone waveforms, without first
expanding the published `m5-tone-output` contract.

The metronome proved that the 0.1.3 package is reliable for synchronized tone
onsets, but intensive timbre tuning exposed needs that a sustained piano voice
did not: envelopes, noise, pitch contours, and possibly sample playback. This
probe gathers hardware evidence before choosing an API.

## Design

`apps/plus2-click-renderer-probe` compares three renderers:

1. the existing package's 294 Hz `saw32` tone baseline;
2. a 16 kHz signed 8-bit noise transient with a linear decay;
3. a 16 kHz signed 8-bit 720 Hz resonant body mixed with a noisy strike and a
   squared decay.

All clicks last 45 ms and use volume 96. Button A triggers the selected click
and Button B changes renderer. The app generates both 720-byte PCM buffers once
during setup into fixed-size static storage. Triggering uses M5Unified
`playRaw()` directly and performs no filesystem access, decoding, dynamic
allocation, or sample generation in the event path.

The PCM algorithms are intentionally local experimental material, not package
APIs and not claims of production-quality synthesis. They let the buzzer tell
us whether irregular PCM transients survive its physical response at all.

## Verification target

```sh
just build-click
just upload-click
just monitor-click
```

On hardware:

1. run the overlap burst for each renderer and confirm all eight triggers
   report `ok=yes`;
2. compare perceived beepiness, attack definition, loudness, and comfort;
3. confirm the 10 ms burst replaces each 45 ms sound rather than queuing it;
4. determine whether either PCM click offers a useful character unavailable
   from the saw tone;
5. note whether the two PCM designs remain meaningfully distinct through the
   buzzer.

This slice answers sound viability and basic trigger behavior. A later slice
can schedule the winning candidates on a clock to measure interaction with a
time-sensitive consumer.

## Hardware result

The M5StickC Plus2 produced three clearly distinct sounds. The saw renderer was
recognizably tonal, the noise renderer was recognizably noisy, and the
wood-style PCM renderer produced a discrete click. The buzzer therefore
preserves useful differences between short PCM shapes rather than collapsing
all of them into the same resonant beep.

This validates fixed, precomputed PCM one-shots as a credible direction for
natural metronome transients. It does not yet establish a package contract or
prove repeated clocked playback; rapid replacement behavior remains to be
checked before closing this probe slice.

Manual button mashing subsequently produced a long sequence of successful wood
PCM triggers, all with `ok=yes`. The shortest observed human interval was about
110 ms, however, so those presses did not overlap the 45 ms sample.

The probe now makes that stress deterministic: one Button A press schedules
eight triggers 10 ms apart using non-blocking absolute deadlines. The second
through eighth triggers necessarily arrive during the preceding sample. This
tests the `stop_current_sound=true` replacement path directly and logs a burst
index for every attempt.

Hardware bursts for both noise and wood PCM returned `ok=yes` for every
overlapping trigger. Replacement is therefore robust under deliberately
unrealistic retrigger pressure: there were no rejected starts, crashes, or
eventual loss of output.

The timestamps also exposed a performance boundary. Although deadlines were
spaced by 10 ms, most observed trigger gaps were about 12-13 ms. In one wood
burst, accumulated lateness caused the last two observations to occur only
1 ms apart. Because `now_ms` is captured before `playRaw()`, this pattern is
consistent with replacement/setup occupying a substantial part of the loop
under continuous retriggering; the simple scheduler falls behind and then
catches up.

This does not threaten ordinary metronome playback, where even 300 BPM leaves
200 ms between quarter-note clicks. It does mean a future abstraction should
not promise sample-accurate rapid retriggering merely because `playRaw()` is
non-blocking with respect to the full 45 ms playback. Clocked onset behavior at
real musical intervals remains the relevant next validation.
