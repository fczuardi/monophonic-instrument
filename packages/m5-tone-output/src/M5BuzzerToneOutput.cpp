#include "M5BuzzerToneOutput.h"

#include <M5Unified.h>

M5BuzzerToneOutput::M5BuzzerToneOutput(AudioIdlePolicy idlePolicy)
    : M5ToneOutputCore(
          SPEAKER_VOLUME,
          {MIN_VELOCITY_VOLUME, MAX_VELOCITY_VOLUME},
          ToneWaveform::Saw32,
          idlePolicy) {
}

bool M5BuzzerToneOutput::begin() {
  auto config = M5.Speaker.config();
  config.buzzer = true;
  config.pin_data_out = BUZZER_GPIO_PIN;
  config.sample_rate = SPEAKER_SAMPLE_RATE_HZ;
  config.magnification = SPEAKER_MAGNIFICATION;
  M5.Speaker.config(config);

  return M5ToneOutputCore::begin();
}
