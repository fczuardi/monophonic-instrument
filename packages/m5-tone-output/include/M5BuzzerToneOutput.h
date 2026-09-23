#pragma once

#include <cstdint>

#include "M5ToneOutputCore.h"
#include "ToneWaveform.h"

// M5StickC Plus2 buzzer output backed by M5Unified's Speaker abstraction.
class M5BuzzerToneOutput : public M5ToneOutputCore {
public:
  explicit M5BuzzerToneOutput(
      AudioIdlePolicy idlePolicy = AudioIdlePolicy::StopWhenIdle);

  bool begin();

private:
  static constexpr uint8_t BUZZER_GPIO_PIN = 2;
  static constexpr uint8_t SPEAKER_VOLUME = 128;
  static constexpr uint8_t MIN_VELOCITY_VOLUME = 64;
  static constexpr uint8_t MAX_VELOCITY_VOLUME = 128;
  static constexpr uint8_t SPEAKER_MAGNIFICATION = 32;
  static constexpr uint32_t SPEAKER_SAMPLE_RATE_HZ = 48000;
};
