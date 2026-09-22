#include "GeneratedClickLibrary.h"

#include <cmath>
#include <cstdint>

namespace {
constexpr uint32_t SAMPLE_RATE_HZ = 16000;
constexpr size_t MAX_SAMPLE_COUNT = 2560;
constexpr float FULL_CYCLE_RADIANS = 6.28318530718f;

constexpr size_t GENERATED_CLICK_SAMPLES = 2560;  // 160 ms

int8_t lowKnock[MAX_SAMPLE_COUNT];
int8_t woodBlock[MAX_SAMPLE_COUNT];
int8_t brightClave[MAX_SAMPLE_COUNT];
int8_t rimBlend[MAX_SAMPLE_COUNT];
int8_t dryImpulse[MAX_SAMPLE_COUNT];
int8_t noiseTap[MAX_SAMPLE_COUNT];
int8_t brushNoise[MAX_SAMPLE_COUNT];
int8_t downSweep[MAX_SAMPLE_COUNT];

const PcmS8Sample SAMPLES[] = {
    {lowKnock, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {woodBlock, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {brightClave, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {rimBlend, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {dryImpulse, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {noiseTap, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {brushNoise, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
    {downSweep, GENERATED_CLICK_SAMPLES, SAMPLE_RATE_HZ},
};

const char* NAMES[] = {
    "low knock",
    "wood block",
    "bright clave",
    "rim blend",
    "dry impulse",
    "noise tap",
    "brush noise",
    "down sweep",
};

uint32_t noiseState = 0x6D2B79F5u;

int8_t nextNoise() {
  noiseState ^= noiseState << 13;
  noiseState ^= noiseState >> 17;
  noiseState ^= noiseState << 5;
  return static_cast<int8_t>(noiseState >> 24);
}

float squaredEnvelope(size_t index, size_t sampleCount) {
  const float remaining =
      1.0f - static_cast<float>(index) / sampleCount;
  return remaining * remaining;
}

int8_t clampSample(float value) {
  if (value > 127.0f) return 127;
  if (value < -128.0f) return -128;
  return static_cast<int8_t>(value);
}

void buildLowKnock() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float seconds = static_cast<float>(index) / SAMPLE_RATE_HZ;
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    const float body =
        std::sin(FULL_CYCLE_RADIANS * 260.0f * seconds) * 112.0f;
    lowKnock[index] = clampSample(body * envelope);
  }
}

void buildWoodBlock() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float seconds = static_cast<float>(index) / SAMPLE_RATE_HZ;
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    const float body =
        std::sin(FULL_CYCLE_RADIANS * 720.0f * seconds) * 82.0f;
    const float strike = static_cast<float>(nextNoise()) * 0.22f;
    woodBlock[index] = clampSample((body + strike) * envelope);
  }
}

void buildBrightClave() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float seconds = static_cast<float>(index) / SAMPLE_RATE_HZ;
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    const float tone =
        std::sin(FULL_CYCLE_RADIANS * 1760.0f * seconds) * 112.0f;
    brightClave[index] = clampSample(tone * envelope);
  }
}

void buildRimBlend() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float seconds = static_cast<float>(index) / SAMPLE_RATE_HZ;
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    const float low =
        std::sin(FULL_CYCLE_RADIANS * 940.0f * seconds) * 68.0f;
    const float high =
        std::sin(FULL_CYCLE_RADIANS * 2310.0f * seconds) * 46.0f;
    const float strike = static_cast<float>(nextNoise()) * 0.14f;
    rimBlend[index] = clampSample((low + high + strike) * envelope);
  }
}

void buildDryImpulse() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    dryImpulse[index] = 0;
  }
  constexpr size_t IMPULSE_SPACING_SAMPLES = 160;  // 10 ms
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES;
       index += IMPULSE_SPACING_SAMPLES) {
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    dryImpulse[index] = clampSample(127.0f * envelope);
    dryImpulse[index + 1] = clampSample(-128.0f * envelope);
  }
}

void buildNoiseTap() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    noiseTap[index] =
        clampSample(static_cast<float>(nextNoise()) * envelope * 0.9f);
  }
}

void buildBrushNoise() {
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float remaining =
        1.0f - static_cast<float>(index) / GENERATED_CLICK_SAMPLES;
    brushNoise[index] =
        clampSample(static_cast<float>(nextNoise()) * remaining * 0.62f);
  }
}

void buildDownSweep() {
  float phase = 0.0f;
  for (size_t index = 0; index < GENERATED_CLICK_SAMPLES; index++) {
    const float position =
        static_cast<float>(index) / GENERATED_CLICK_SAMPLES;
    const float frequencyHz = 1900.0f - 1500.0f * position;
    phase += FULL_CYCLE_RADIANS * frequencyHz / SAMPLE_RATE_HZ;
    const float envelope = squaredEnvelope(index, GENERATED_CLICK_SAMPLES);
    downSweep[index] = clampSample(std::sin(phase) * 104.0f * envelope);
  }
}
}  // namespace

void buildGeneratedClickLibrary() {
  noiseState = 0x6D2B79F5u;
  buildLowKnock();
  buildWoodBlock();
  buildBrightClave();
  buildRimBlend();
  buildDryImpulse();
  buildNoiseTap();
  buildBrushNoise();
  buildDownSweep();
}

size_t generatedClickCount() {
  return static_cast<size_t>(GeneratedClickId::Count);
}

const char* generatedClickName(GeneratedClickId id) {
  const size_t index = static_cast<size_t>(id);
  return index < generatedClickCount() ? NAMES[index] : "unknown";
}

const PcmS8Sample& generatedClickSample(GeneratedClickId id) {
  const size_t index = static_cast<size_t>(id);
  return SAMPLES[index < generatedClickCount() ? index : 0];
}
