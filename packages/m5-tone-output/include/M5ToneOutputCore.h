#pragma once

#include <cstddef>
#include <cstdint>

#include "AudioIdlePolicy.h"
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
      ToneWaveform waveform,
      AudioIdlePolicy idlePolicy);

  bool begin();
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
  bool setIdlePolicy(AudioIdlePolicy idlePolicy);
  AudioIdlePolicy idlePolicy() const;
  void setVelocityVolumeRange(VelocityVolumeRange range);
  VelocityVolumeRange velocityVolumeRange() const;
  uint8_t volumeForVelocity(uint8_t velocity) const;
  void setWaveform(ToneWaveform waveform);
  ToneWaveform waveform() const;
  const char* waveformName() const;

private:
  static constexpr int SPEAKER_CHANNEL = 0;
  static constexpr int KEEP_ALIVE_CHANNEL = 1;
  static constexpr uint32_t KEEP_ALIVE_SAMPLE_RATE_HZ = 16000;
  static constexpr size_t KEEP_ALIVE_SAMPLE_COUNT = 256;

  bool startKeepAlive();
  void stopKeepAlive();

  bool initialized_ = false;
  bool keepAliveActive_ = false;
  uint8_t volume_;
  VelocityVolumeRange velocityVolumeRange_;
  ToneWaveform waveform_;
  AudioIdlePolicy idlePolicy_;
  int8_t keepAliveSilence_[KEEP_ALIVE_SAMPLE_COUNT] = {};
};
