# Plus2 click renderer probe

M5StickC Plus2 listening probe for comparing the published tone backend with
app-local PCM one-shots before changing any package contract.

- Button A triggers the selected click once.
- Button B selects `saw tone`, `noise PCM`, or `wood PCM`.

The two 45 ms PCM buffers are generated once during setup into fixed-size
storage. Playback performs no filesystem access, decoding, or allocation.

```sh
just build-click
just upload-click
just monitor-click
```
