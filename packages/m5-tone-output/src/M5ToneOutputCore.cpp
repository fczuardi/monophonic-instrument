#include "M5ToneOutputCore.h"

#include <Arduino.h>
#include <M5Unified.h>

#include "ToneWaveformSamples.h"

M5ToneOutputCore::M5ToneOutputCore(
    uint8_t initialVolume,
    VelocityVolumeRange velocityVolumeRange,
    ToneWaveform waveform,
    AudioIdlePolicy idlePolicy)
    : volume_(initialVolume),
      velocityVolumeRange_(velocityVolumeRange),
      waveform_(waveform),
      idlePolicy_(idlePolicy) {
}

bool M5ToneOutputCore::begin() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(volume_);
  initialized_ = true;
  return idlePolicy_ != AudioIdlePolicy::KeepAlive || startKeepAlive();
}

void M5ToneOutputCore::end() {
  if (!initialized_) {
    return;
  }

  stop();
  stopKeepAlive();
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

bool M5ToneOutputCore::setIdlePolicy(AudioIdlePolicy idlePolicy) {
  idlePolicy_ = idlePolicy;
  if (!initialized_) return true;

  if (idlePolicy_ == AudioIdlePolicy::KeepAlive) return startKeepAlive();

  stopKeepAlive();
  return true;
}

AudioIdlePolicy M5ToneOutputCore::idlePolicy() const {
  return idlePolicy_;
}

bool M5ToneOutputCore::startKeepAlive() {
  if (keepAliveActive_) return true;

  keepAliveActive_ = M5.Speaker.playRaw(
      keepAliveSilence_, KEEP_ALIVE_SAMPLE_COUNT, KEEP_ALIVE_SAMPLE_RATE_HZ,
      false, UINT32_MAX, KEEP_ALIVE_CHANNEL, true);
  return keepAliveActive_;
}

void M5ToneOutputCore::stopKeepAlive() {
  if (!keepAliveActive_) return;
  M5.Speaker.stop(KEEP_ALIVE_CHANNEL);
  keepAliveActive_ = false;
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
