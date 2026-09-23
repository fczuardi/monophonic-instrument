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

## Implementation decision

The metronome has already reproduced the problem and demonstrated the useful
solution. No additional probe is required before changing the package. Add the
idle policy directly to `m5-tone-output`, then use the metronome as its primary
hardware validation consumer.

Expose two policies:

- `stop when idle`: current 0.1.4 behavior;
- `keep alive`: loop zero-valued signed 8-bit PCM on a separate channel.

The implementation and its validation should exercise at least:

- a zero-valued sample, to reveal lifecycle transients without musical PCM;
- a low-frequency damped sample, where the transient previously dominated;
- a high-frequency sample, representative of successful metronome clicks;
- master volumes 0, 32, 64, 128, and 255;
- isolated triggers and rapid replacement on the package-owned channel;
- transition into and out of keep-alive while no one-shot is active.

Serial records must identify policy, source sample, volume, output channel,
request success, and time. Acoustic conclusions remain human observations;
serial success cannot measure physical onset or transient amplitude.

## Lifecycle and coexistence checks

Implement the small API first, then use these checks to refine it:

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

## Initial package contract

Start with an explicit policy:

```cpp
enum class AudioIdlePolicy : uint8_t {
  StopWhenIdle,
  KeepAlive,
};
```

The initial implementation may choose construction or pre-`begin()`
configuration according to whichever keeps the code simplest. Backward
compatibility with 0.1.4 is not a constraint; update every in-workspace consumer
if the clearer API breaks it. Semantics are:

- `StopWhenIdle` allows the speaker path to idle;
- `KeepAlive` owns and maintains the silent backend channel;
- `stop()` stops the musical channel but preserves the configured idle policy;
- `end()` always stops both musical and keep-alive channels;
- failed keep-alive startup is observable;
- repeated configuration and lifecycle calls are safe and deterministic.

The channel number and silence-buffer details should remain implementation
details unless hardware testing proves they must be configurable.

## Verification and release

After the contract is implemented:

```sh
just check
pio run -d ci/consumers/m5-tone-output
```

Update and rebuild existing tone/instrument probes when the API changes, then
perform the relevant hardware checks. Publish a new package snapshot once the
implementation works well enough for the metronome; further breaking revisions
remain acceptable.

Finally, update the metronome to consume the released policy and remove all
direct `M5.Speaker` channel ownership from `MetronomeAudio`. That consumer build
and hardware run are the end-to-end acceptance test for the boundary.

## Implementation checkpoint

The first API is now implemented directly:

- `AudioIdlePolicy::{StopWhenIdle, KeepAlive}`;
- policy selection in the concrete output constructors;
- `begin()` reports whether the requested policy was established;
- `setIdlePolicy()` switches behavior before or after startup;
- `stop()` affects only the musical channel;
- `end()` stops both the musical and keep-alive channels;
- the silent buffer and channel ownership remain private to
  `M5ToneOutputCore`.

The clean package consumer compiles both policies and runtime switching. The
metronome also builds against the sibling package with its direct
`M5.Speaker.playRaw()` workaround removed; RAM remains 16.4%. Hardware listening
is now the remaining check that package-owned keep-alive preserves the already
validated absence of the electrical start transient.

## Hardware result

The migrated metronome reported
`audio: idle_policy=keep_alive ok=yes` on the M5StickC Plus2. The electrical
start transient remained absent, master-gain changes affected both the
plain-1600 accent and mid-tick regular click, and volume zero was silent while
PCM requests continued normally.

Beat dispatch remained on its exact 500 ms phase throughout the supplied idle
and volume-adjustment logs, with ordinary beats reporting `elapsed=1` and every
sample request reporting `ok=yes`. This validates the package-owned policy and
the removal of direct channel ownership from the application.

## Non-goals

This plan does not add synthesis recipes, per-sample gain, mixing, arbitrary
channel allocation, or a general audio engine. It addresses one observed
backend responsibility: predictable PCM one-shot behavior across idle periods.
