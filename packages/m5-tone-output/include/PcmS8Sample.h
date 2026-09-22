#pragma once

#include <cstddef>
#include <cstdint>

// Non-owning description of mono signed 8-bit PCM audio.
//
// The caller owns data and must keep it valid and unchanged until playback
// finishes or the output is stopped.
struct PcmS8Sample {
  const int8_t* data;
  size_t sampleCount;
  uint32_t sampleRateHz;
};
