# PCM idle-policy improvement plan

## Motivation

The M5StickC Plus2 metronome became the second real consumer of
`m5-tone-output` PCM one-shots. It exposed an output-lifecycle behavior that the
button-triggered click probe had not isolated: starting a one-shot after the
speaker path becomes idle can add a repeatable physical/electrical transient.

That transient was especially misleading with a low-frequency 260 Hz recipe.
It sounded like an aggressive attack, remained perceptually constant as master
volume fell, and survived changes to the PCM attack envelope. Logging confirmed
that M5Unified's master volume really reached zero, so the state was not being
lost. Looping a zero-valued PCM buffer on a second virtual channel kept the
output path active; the invariant transient disappeared and master gain then
affected the musical waveform normally.

The experiment also ruled out app-side signed-8-bit gain as a general fix.
Squaring a low volume and quantizing back to `int8_t` destroyed waveform detail
before playback. With continuous output active, the existing M5Unified master
gain preserves the original PCM and behaves better.

## Boundary

Speaker lifecycle and virtual-channel ownership belong to the audio backend,
not to each musical application. The metronome currently owns a direct
M5Unified channel-1 silent loop as a temporary probe. A validated package
feature should eventually replace that workaround.

Musical sound recipes, accent selection, and judgments about useful frequency
ranges remain application concerns and do not move into `m5-tone-output`.

## Stage 1: dedicated reproduction probe

Add a small M5StickC Plus2 app that uses the local package and can switch
between two policies without reflashing:

- `stop when idle`: current 0.1.4 behavior;
- `keep alive`: loop zero-valued signed 8-bit PCM on a separate channel.

The probe should exercise at least:

- a zero-valued sample, to reveal lifecycle transients without musical PCM;
- a low-frequency damped sample, where the transient previously dominated;
- a high-frequency sample, representative of successful metronome clicks;
- master volumes 0, 32, 64, 128, and 255;
- isolated triggers and rapid replacement on the package-owned channel;
- transition into and out of keep-alive while no one-shot is active.

Serial records must identify policy, source sample, volume, output channel,
request success, and time. Acoustic conclusions remain human observations;
serial success cannot measure physical onset or transient amplitude.

## Stage 2: lifecycle and coexistence checks

Before proposing an API, establish:

1. whether a zero loop reliably prevents the transient after long idle periods;
2. whether channel 1 can be reserved without interfering with channel-0 tones
   and PCM replacement;
3. whether changing master volume affects both the silent stream and one-shots
   without introducing new transitions;
4. whether enabling and disabling keep-alive is itself audible;
5. whether repeated `begin()`/`end()` cycles stop and release both channels;
6. whether tone playback and `stop()` retain their existing semantics;
7. the practical power/battery cost of keeping the speaker task active.

Power cost may initially be recorded qualitatively if current measurement is
not available, but it must remain an explicit tradeoff.

## Candidate package contract

If the probe validates the behavior, prefer an explicit opt-in policy with the
current behavior as the backward-compatible default:

```cpp
enum class AudioIdlePolicy : uint8_t {
  StopWhenIdle,
  KeepAlive,
};
```

Configuration may be supplied at construction or before `begin()`. The final
shape should make ownership clear without exposing raw M5Unified channels.
Required semantics are:

- `StopWhenIdle` matches version 0.1.4;
- `KeepAlive` owns and maintains the silent backend channel;
- `stop()` stops the musical channel but preserves the configured idle policy;
- `end()` always stops both musical and keep-alive channels;
- failed keep-alive startup is observable;
- repeated configuration and lifecycle calls are safe and deterministic.

The channel number and silence-buffer details should remain implementation
details unless hardware testing proves they must be configurable.

## Stage 3: verification and release

After the contract is implemented:

```sh
just check
pio run -d ci/consumers/m5-tone-output
```

Also rebuild existing tone/instrument probes to protect source compatibility,
then perform the dedicated hardware matrix. A new package version should be
published only after those checks and its README documents the power/latency
tradeoff.

Finally, update the metronome to consume the released policy and remove all
direct `M5.Speaker` channel ownership from `MetronomeAudio`. That consumer build
and hardware run are the end-to-end acceptance test for the boundary.

## Non-goals

This plan does not add synthesis recipes, per-sample gain, mixing, arbitrary
channel allocation, or a general audio engine. It addresses one observed
backend responsibility: predictable PCM one-shot behavior across idle periods.
