#include "M5CoreGrayToneOutput.h"

M5CoreGrayToneOutput::M5CoreGrayToneOutput(AudioIdlePolicy idlePolicy)
    : M5ToneOutputCore(
          SPEAKER_VOLUME,
          {MIN_VELOCITY_VOLUME, MAX_VELOCITY_VOLUME},
          ToneWaveform::Square32,
          idlePolicy) {
}
