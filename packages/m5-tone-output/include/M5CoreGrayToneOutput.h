#pragma once

#include <cstdint>

#include "M5ToneOutputCore.h"
#include "ToneWaveform.h"

// M5Stack Core Gray internal speaker output backed by M5Unified.
class M5CoreGrayToneOutput : public M5ToneOutputCore {
public:
  explicit M5CoreGrayToneOutput(
      AudioIdlePolicy idlePolicy = AudioIdlePolicy::StopWhenIdle);

private:
  static constexpr uint8_t SPEAKER_VOLUME = 35;
  static constexpr uint8_t MIN_VELOCITY_VOLUME = 1;
  static constexpr uint8_t MAX_VELOCITY_VOLUME = 35;
};
