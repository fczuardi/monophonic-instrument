#include "M5ToneOutputCore.h"

#include <Arduino.h>
#include <M5Unified.h>

#include "ToneWaveformSamples.h"

M5ToneOutputCore::M5ToneOutputCore(
    uint8_t initialVolume,
    VelocityVolumeRange velocityVolumeRange,
    ToneWaveform waveform)
    : volume_(initialVolume),
      velocityVolumeRange_(velocityVolumeRange),
      waveform_(waveform) {
}

void M5ToneOutputCore::begin() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(volume_);
  initialized_ = true;
}

void M5ToneOutputCore::end() {
  if (!initialized_) {
    return;
  }

  stop();
  M5.Speaker.end();
  initialized_ = false;
}

bool M5ToneOutputCore::startTone(float frequencyHz) {
  if (!initialized_ || frequencyHz <= 0.0f) {
    return false;
  }

  const ToneWaveformSamples waveformSamples = toneWaveformSamples(waveform_);
  const bool toneStarted = M5.Speaker.tone(
      frequencyHz,
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      waveformSamples.samples,
      waveformSamples.sampleCount);

  return toneStarted;
}

bool M5ToneOutputCore::playSample(const PcmS8Sample& sample) {
  if (!initialized_ || sample.data == nullptr || sample.sampleCount == 0 ||
      sample.sampleRateHz == 0) {
    return false;
  }

  return M5.Speaker.playRaw(
      sample.data,
      sample.sampleCount,
      sample.sampleRateHz,
      false,
      1,
      SPEAKER_CHANNEL,
      true);
}

bool M5ToneOutputCore::startNote(
    uint8_t,
    float frequencyHz,
    ToneWaveform waveform,
    uint8_t velocity) {
  setWaveform(waveform);
  setVolume(volumeForVelocity(velocity));
  return startTone(frequencyHz);
}

void M5ToneOutputCore::stop() {
  if (!initialized_) {
    return;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
}

void M5ToneOutputCore::setVolume(uint8_t volume) {
  volume_ = volume;
  if (initialized_) {
    M5.Speaker.setVolume(volume_);
  }
}

uint8_t M5ToneOutputCore::volume() const {
  return volume_;
}

void M5ToneOutputCore::setVelocityVolumeRange(VelocityVolumeRange range) {
  if (range.minimum > range.maximum) {
    const uint8_t originalMinimum = range.minimum;
    range.minimum = range.maximum;
    range.maximum = originalMinimum;
  }

  velocityVolumeRange_ = range;
}

VelocityVolumeRange M5ToneOutputCore::velocityVolumeRange() const {
  return velocityVolumeRange_;
}

uint8_t M5ToneOutputCore::volumeForVelocity(uint8_t velocity) const {
  return velocityVolumeRange_.minimum +
      ((static_cast<uint16_t>(velocity) *
        (velocityVolumeRange_.maximum - velocityVolumeRange_.minimum)) /
       127);
}

void M5ToneOutputCore::stopNote() {
  stop();
}

bool M5ToneOutputCore::isPlaying() const {
  return initialized_ && M5.Speaker.isPlaying(SPEAKER_CHANNEL) > 0;
}

void M5ToneOutputCore::setWaveform(ToneWaveform waveform) {
  waveform_ = waveform;
}

ToneWaveform M5ToneOutputCore::waveform() const {
  return waveform_;
}

const char* M5ToneOutputCore::waveformName() const {
  return toneWaveformName(waveform_);
}
