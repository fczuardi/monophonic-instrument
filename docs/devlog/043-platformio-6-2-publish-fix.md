# PlatformIO 6.2 publish fix

## Goal

Restore the package publication workflow after the `m5-tone-output-v0.1.4`
run failed while building its isolated consumer.

## Failure

GitHub Actions run `35788488498` installed the workflow's pinned PlatformIO
Core 6.1.19, then resolved the pioarduino stable ESP32 platform to version
55.3.312. That platform now declares PlatformIO Core 6.2.0 as its minimum and
aborted with `IncompatiblePlatform` before package packing or publication.

The failure was therefore in CI tool compatibility, not the package source,
consumer fixture, registry credentials, or immutable version state.

## Change

The workflow now pins PlatformIO Core 6.2.0, matching both the current
pioarduino requirement and the locally installed Core used by the successful
consumer builds. The pin remains exact so release behavior does not drift
implicitly.

Because the original tag workflow failed before its publish step, version
0.1.4 was not submitted to the registry. After this fix reaches the remote,
the release tag can be moved to the fix commit and pushed with force; the
replacement workflow will then package the same `m5-tone-output` 0.1.4 source
through the compatible toolchain.

## Verification

The local environment reports PlatformIO Core 6.2.0. Before the original tag,
the repository's full check passed all 51 native tests and built the isolated
package consumer plus all hardware applications. The replacement tag run is
the final verification of the clean GitHub runner path.
