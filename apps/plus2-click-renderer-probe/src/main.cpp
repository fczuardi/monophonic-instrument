#include <Arduino.h>
#include <M5Unified.h>

#include <cmath>
#include <cstdint>

#include "M5BuzzerToneOutput.h"

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t SAMPLE_RATE_HZ = 16000;
constexpr uint32_t CLICK_DURATION_MS = 45;
constexpr uint32_t BURST_INTERVAL_MS = 10;
constexpr uint8_t BURST_TRIGGER_COUNT = 8;
constexpr size_t CLICK_SAMPLE_COUNT =
    SAMPLE_RATE_HZ * CLICK_DURATION_MS / 1000;
constexpr float TONE_FREQUENCY_HZ = 294.0f;
constexpr uint8_t OUTPUT_VOLUME = 96;
constexpr int SPEAKER_CHANNEL = 0;
constexpr float FULL_CYCLE_RADIANS = 6.28318530718f;

enum class Renderer : uint8_t {
  SawTone,
  NoisePcm,
  WoodPcm,
};

M5BuzzerToneOutput toneOutput;
Renderer selectedRenderer = Renderer::SawTone;
int8_t noiseClick[CLICK_SAMPLE_COUNT];
int8_t woodClick[CLICK_SAMPLE_COUNT];
uint32_t toneStopAtMs = 0;
bool toneActive = false;
uint32_t nextBurstAtMs = 0;
uint8_t burstTriggerIndex = 0;
bool burstActive = false;

const char* rendererName(Renderer renderer) {
  switch (renderer) {
    case Renderer::SawTone:
      return "saw tone";
    case Renderer::NoisePcm:
      return "noise PCM";
    case Renderer::WoodPcm:
      return "wood PCM";
  }
  return "unknown";
}

void buildClickBuffers() {
  uint32_t noiseState = 0x6D2B79F5u;

  for (size_t index = 0; index < CLICK_SAMPLE_COUNT; index++) {
    noiseState ^= noiseState << 13;
    noiseState ^= noiseState >> 17;
    noiseState ^= noiseState << 5;

    const int32_t noise = static_cast<int8_t>(noiseState >> 24);
    const int32_t samplesRemaining = CLICK_SAMPLE_COUNT - index;
    const int32_t linearEnvelope =
        (samplesRemaining * 110) / CLICK_SAMPLE_COUNT;
    noiseClick[index] = static_cast<int8_t>(
        (noise * linearEnvelope) / 128);

    const float seconds = static_cast<float>(index) / SAMPLE_RATE_HZ;
    const float normalizedPosition =
        static_cast<float>(index) / CLICK_SAMPLE_COUNT;
    const float envelope = (1.0f - normalizedPosition) *
                           (1.0f - normalizedPosition);
    const float body =
        std::sin(FULL_CYCLE_RADIANS * 720.0f * seconds) * 82.0f;
    const float strike = static_cast<float>(noise) * 0.22f;
    woodClick[index] = static_cast<int8_t>((body + strike) * envelope);
  }
}

void drawScreen(const char* status) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(8, 8);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("CLICK PROBE");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.printf("Renderer: %s\n", rendererName(selectedRenderer));
  M5.Display.printf("Length: %lu ms\n",
                    static_cast<unsigned long>(CLICK_DURATION_MS));
  M5.Display.println();
  M5.Display.println("A: overlap burst");
  M5.Display.println("B: next renderer");
  M5.Display.println();
  M5.Display.printf("Status: %s\n", status);
}

void stopToneWhenDue(uint32_t nowMs) {
  if (!toneActive || static_cast<int32_t>(nowMs - toneStopAtMs) < 0) return;

  toneOutput.stop();
  toneActive = false;
}

bool triggerSelected(uint32_t nowMs, uint8_t burstIndex) {
  toneOutput.stop();
  toneActive = false;

  bool started = false;
  switch (selectedRenderer) {
    case Renderer::SawTone:
      started = toneOutput.startTone(TONE_FREQUENCY_HZ);
      toneActive = started;
      toneStopAtMs = nowMs + CLICK_DURATION_MS;
      break;
    case Renderer::NoisePcm:
      started = M5.Speaker.playRaw(
          noiseClick, CLICK_SAMPLE_COUNT, SAMPLE_RATE_HZ, false, 1,
          SPEAKER_CHANNEL, true);
      break;
    case Renderer::WoodPcm:
      started = M5.Speaker.playRaw(
          woodClick, CLICK_SAMPLE_COUNT, SAMPLE_RATE_HZ, false, 1,
          SPEAKER_CHANNEL, true);
      break;
  }

  Serial.printf(
      "click_probe: action=trigger renderer=%s duration_ms=%lu "
      "sample_rate_hz=%lu samples=%u burst_index=%u ok=%s now_ms=%lu\n",
      rendererName(selectedRenderer),
      static_cast<unsigned long>(CLICK_DURATION_MS),
      static_cast<unsigned long>(SAMPLE_RATE_HZ),
      static_cast<unsigned>(CLICK_SAMPLE_COUNT), burstIndex,
      started ? "yes" : "no",
      static_cast<unsigned long>(nowMs));
  return started;
}

void startBurst() {
  burstActive = true;
  burstTriggerIndex = 0;
  Serial.printf(
      "click_probe: action=burst_start renderer=%s count=%u interval_ms=%lu\n",
      rendererName(selectedRenderer), BURST_TRIGGER_COUNT,
      static_cast<unsigned long>(BURST_INTERVAL_MS));
  drawScreen("burst running");
  // Start the timing window after display work so redraw latency is not
  // mistaken for audio scheduling jitter.
  nextBurstAtMs = millis();
}

void serviceBurst(uint32_t nowMs) {
  if (!burstActive || static_cast<int32_t>(nowMs - nextBurstAtMs) < 0) return;

  burstTriggerIndex++;
  triggerSelected(nowMs, burstTriggerIndex);

  if (burstTriggerIndex >= BURST_TRIGGER_COUNT) {
    burstActive = false;
    Serial.printf("click_probe: action=burst_complete renderer=%s\n",
                  rendererName(selectedRenderer));
    drawScreen("burst complete");
    return;
  }

  nextBurstAtMs += BURST_INTERVAL_MS;
}

void selectNextRenderer() {
  toneOutput.stop();
  toneActive = false;
  burstActive = false;
  const uint8_t next =
      (static_cast<uint8_t>(selectedRenderer) + 1) % 3;
  selectedRenderer = static_cast<Renderer>(next);
  Serial.printf("click_probe: action=select renderer=%s\n",
                rendererName(selectedRenderer));
  drawScreen("selected");
}
}  // namespace

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  config.fallback_board = m5::board_t::board_M5StickCPlus2;
  M5.begin(config);

  Serial.begin(SERIAL_BAUD);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);
  toneOutput.begin();
  toneOutput.setVolume(OUTPUT_VOLUME);
  buildClickBuffers();
  drawScreen("ready");

  Serial.printf(
      "click_probe: ready sample_rate_hz=%lu duration_ms=%lu samples=%u "
      "bytes_per_buffer=%u volume=%u\n",
      static_cast<unsigned long>(SAMPLE_RATE_HZ),
      static_cast<unsigned long>(CLICK_DURATION_MS),
      static_cast<unsigned>(CLICK_SAMPLE_COUNT),
      static_cast<unsigned>(sizeof(noiseClick)), OUTPUT_VOLUME);
}

void loop() {
  M5.update();
  const uint32_t nowMs = millis();
  stopToneWhenDue(nowMs);

  if (M5.BtnA.wasPressed()) startBurst();
  if (M5.BtnB.wasClicked()) selectNextRenderer();

  serviceBurst(nowMs);

  delay(1);
}
