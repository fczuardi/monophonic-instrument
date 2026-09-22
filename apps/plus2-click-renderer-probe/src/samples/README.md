# Converted metronome samples

These generated headers contain selected sounds from Ludwig Peter Müller's
December 2020 metronome recording collection, released under CC0 1.0
Universal. The original collection identifies `muellerwig@gmail.com` as the
contact.

Included pairs:

- `Perc_MetronomeQuartz_lo.wav` / `_hi.wav`
- `Perc_Stick_lo.wav` / `_hi.wav`
- `Perc_Castanet_lo.wav` / `_hi.wav`

Each 48 kHz stereo signed 16-bit WAV was converted uniformly to 16 kHz mono
signed 8-bit raw PCM with 12 dB gain, retaining the complete approximately
170 ms source duration:

```sh
ffmpeg -i SOURCE.wav -af volume=12dB -ac 1 -ar 16000 -f s8 OUTPUT.raw
```

The raw bytes are represented as generated `uint8_t` arrays so every bit
pattern can be expressed without C++ narrowing errors. The probe reinterprets
the immutable storage as signed 8-bit PCM when constructing `PcmS8Sample`.
