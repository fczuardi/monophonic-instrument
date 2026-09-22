# Converted metronome sample probe

## Goal

Test whether the M5StickC Plus2 buzzer preserves useful character from real
metronome WAV recordings after conversion to the new `PcmS8Sample` format.
Synthetic PCM proved that arbitrary transient shapes remain distinguishable,
but it did not establish whether recorded clicks can sound recognizably similar
or musically useful on this limited transducer.

## Selection

The source `Metronomes` collection contains many low/high pairs. Rather than
convert the complete set, this slice selects three contrasting families:

- quartz metronome: a conventional reference;
- stick: a dry wooden transient;
- castanet: a brighter acoustic transient.

Both low and high variants are included so the same family can be evaluated as
regular and accented beats. The source credits identify Ludwig Peter Müller as
the recorder and release the collection under CC0 1.0 Universal.

## Conversion

Each source is 48 kHz stereo signed 16-bit PCM and approximately 169.6 ms long.
All six files use the same conversion:

```sh
ffmpeg -i SOURCE.wav -af volume=12dB -ac 1 -ar 16000 -f s8 OUTPUT.raw
```

The 12 dB gain moves source peaks from approximately -14.5 dB toward full
scale before 8-bit quantization. Each result contains 2714 bytes, for 16,284
bytes across all six sounds. Complete tails are retained for this listening
probe; shortening can be evaluated only after finding a promising family.

The generated headers live inside the probe app and are not library assets.
They are fixed firmware data passed through `PcmS8Sample`, with no runtime WAV
parsing, conversion, filesystem access, or allocation.

## Interaction and verification

Button B now cycles nine renderers:

1. saw tone;
2. generated noise PCM;
3. generated wood PCM;
4. quartz low;
5. quartz high;
6. stick low;
7. stick high;
8. castanet low;
9. castanet high.

Button A triggers one sound. The screen reports the selected name and actual
duration; Serial reports sample rate, sample count, and playback success.

```sh
just build
just upload-click
just monitor-click
```

The hardware listening target is qualitative: determine which pairs remain
distinct, comfortable, and click-like; whether their low/high relationship is
useful for accents; and whether the 170 ms tail feels appropriate or excessive
at metronome cadence.

## Hardware result

All six converted recordings sounded effectively the same on the M5StickC
Plus2 buzzer. No useful difference was audible between quartz, stick, and
castanet, nor between the low/high member of any pair.

The generated firmware data was checked before attributing this to hardware.
Every raw buffer has a distinct SHA-256 digest and byte content. Their amplitude
statistics also differ: the stick buffers average roughly three times the
absolute sample amplitude of quartz and castanet after the uniform conversion.
The probe therefore was not accidentally replaying one shared buffer, and the
conversion did not produce identical data.

The same signed 8-bit, 16 kHz raw files were then played on a Linux PC through
headphones with FFplay. Quartz, stick, castanet, and their low/high variants
were clearly distinguishable. This listening control confirms that the reduced
PCM format still retains the source differences before reaching the embedded
audio hardware.

The likely explanation is physical: these short recorded attacks excite the
buzzer's own narrow resonant response, which dominates the source spectra and
collapses their audible distinctions. This contrasts with the earlier
algorithmic probe, where a sustained tone, noise burst, and damped wood model
were clearly distinct because they differed in broad signal structure rather
than only in the fine character of similar percussive recordings.

For the Plus2 metronome, arbitrary WAV playback is therefore technically
viable but not a promising route to a library of natural click timbres. The
small generated wood transient remains the stronger candidate: it is distinct,
compact, deterministic, and can be shaped for the transducer. Recorded samples
may still be worthwhile on hardware with a real speaker, but that is a separate
consumer and experiment.
