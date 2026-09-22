#pragma once

#include <cstdint>

#include "PcmS8Sample.h"
#include "ToneWaveform.h"
#include "VoiceOutput.h"

struct VelocityVolumeRange {
  uint8_t minimum;
  uint8_t maximum;
};

// Shared M5Unified speaker-tone behavior for concrete output packages.
//
// Board-specific setup remains outside this class. Callers configure M5.Speaker
// for their hardware before calling begin().
class M5ToneOutputCore : public VoiceOutput {
public:
  M5ToneOutputCore(
      uint8_t initialVolume,
      VelocityVolumeRange velocityVolumeRange,
      ToneWaveform waveform);

  void begin();
  void end();

  bool startNote(
      uint8_t midiNote,
      float frequencyHz,
      ToneWaveform waveform,
      uint8_t velocity) override;
  void stopNote() override;
  bool isPlaying() const override;

  bool startTone(float frequencyHz);
  bool playSample(const PcmS8Sample& sample);
  void stop();
  void setVolume(uint8_t volume);
  uint8_t volume() const;
  void setVelocityVolumeRange(VelocityVolumeRange range);
  VelocityVolumeRange velocityVolumeRange() const;
  uint8_t volumeForVelocity(uint8_t velocity) const;
  void setWaveform(ToneWaveform waveform);
  ToneWaveform waveform() const;
  const char* waveformName() const;

private:
  static constexpr int SPEAKER_CHANNEL = 0;

  bool initialized_ = false;
  uint8_t volume_;
  VelocityVolumeRange velocityVolumeRange_;
  ToneWaveform waveform_;
};
