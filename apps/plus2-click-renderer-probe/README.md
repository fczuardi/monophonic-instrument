# Plus2 click renderer probe

M5StickC Plus2 listening probe for comparing the published tone backend with
app-local PCM one-shots before changing any package contract.

- Button A triggers an eight-click overlap burst at 10 ms intervals.
- Button B selects `saw tone`, `noise PCM`, or `wood PCM`.

Because every sound lasts 45 ms, the burst deliberately replaces active
playback. The two PCM buffers are generated once during setup into fixed-size
storage. Playback performs no filesystem access, decoding, or allocation.

```sh
just build-click
just upload-click
just monitor-click
```
