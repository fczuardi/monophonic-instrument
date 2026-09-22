# Plus2 click renderer probe

M5StickC Plus2 listening lab for comparing a tone reference with an app-local
catalog of generated PCM one-shots.

- Button A triggers the selected click once.
- Button B cycles the saw reference and eight generated sounds.

The generated library covers low knock, wood block, bright clave, rim blend,
dry impulse, noise tap, brush noise, and descending sweep. Buffers are prepared
once during setup into fixed-size storage. Playback performs no filesystem
access, decoding, or allocation.

The earlier converted-recording experiment and its CC0 inputs remain
documented under `src/samples/`, but those samples are no longer part of the
active listening cycle because the Plus2 buzzer made them indistinguishable.

Current metronome finalists from hardware listening are `low knock` for regular
beats and `bright clave` for the downbeat accent.

```sh
just build-click
just upload-click
just monitor-click
```
