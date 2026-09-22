#include <Arduino.h>
#include <M5Unified.h>

#include <cstdint>

#include "GeneratedClickLibrary.h"
#include "M5BuzzerToneOutput.h"

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr float REFERENCE_TONE_FREQUENCY_HZ = 294.0f;
constexpr uint32_t REFERENCE_TONE_DURATION_MS = 45;
constexpr uint8_t OUTPUT_VOLUME = 128;
constexpr uint8_t REFERENCE_RENDERER_COUNT = 1;

M5BuzzerToneOutput toneOutput;
uint8_t selectedRendererIndex = 0;
uint32_t toneStopAtMs = 0;
bool toneActive = false;

size_t rendererCount() {
  return REFERENCE_RENDERER_COUNT + generatedClickCount();
}

bool referenceToneSelected() {
  return selectedRendererIndex == 0;
}

GeneratedClickId selectedGeneratedClick() {
  return static_cast<GeneratedClickId>(selectedRendererIndex - 1);
}

const char* selectedRendererName() {
  return referenceToneSelected()
             ? "saw reference"
             : generatedClickName(selectedGeneratedClick());
}

uint32_t selectedDurationMs() {
  if (referenceToneSelected()) return REFERENCE_TONE_DURATION_MS;

  const PcmS8Sample& sample =
      generatedClickSample(selectedGeneratedClick());
  return sample.sampleCount * 1000UL / sample.sampleRateHz;
}

void drawScreen(const char* status) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(8, 8);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("CLICK LAB");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.printf("Sound %u/%u: %s\n", selectedRendererIndex + 1,
                    static_cast<unsigned>(rendererCount()),
                    selectedRendererName());
  M5.Display.printf("Length: %lu ms\n",
                    static_cast<unsigned long>(selectedDurationMs()));
  M5.Display.println();
  M5.Display.println("A: trigger click");
  M5.Display.println("B: next sound");
  M5.Display.println();
  M5.Display.printf("Status: %s\n", status);
}

void stopToneWhenDue(uint32_t nowMs) {
  if (!toneActive || static_cast<int32_t>(nowMs - toneStopAtMs) < 0) return;

  toneOutput.stop();
  toneActive = false;
}

bool triggerSelected(uint32_t nowMs) {
  toneOutput.stop();
  toneActive = false;

  bool started = false;
  const PcmS8Sample* sample = nullptr;
  if (referenceToneSelected()) {
    started = toneOutput.startTone(REFERENCE_TONE_FREQUENCY_HZ);
    toneActive = started;
    toneStopAtMs = nowMs + REFERENCE_TONE_DURATION_MS;
  } else {
    sample = &generatedClickSample(selectedGeneratedClick());
    started = toneOutput.playSample(*sample);
  }

  Serial.printf(
      "click_lab: action=trigger index=%u name=%s duration_ms=%lu "
      "sample_rate_hz=%lu samples=%u ok=%s now_ms=%lu\n",
      selectedRendererIndex + 1, selectedRendererName(),
      static_cast<unsigned long>(selectedDurationMs()),
      static_cast<unsigned long>(sample == nullptr ? 0 : sample->sampleRateHz),
      static_cast<unsigned>(sample == nullptr ? 0 : sample->sampleCount),
      started ? "yes" : "no", static_cast<unsigned long>(nowMs));
  drawScreen(started ? "triggered" : "failed");
  return started;
}

void selectNextRenderer() {
  toneOutput.stop();
  toneActive = false;
  selectedRendererIndex = (selectedRendererIndex + 1) % rendererCount();
  Serial.printf("click_lab: action=select index=%u name=%s\n",
                selectedRendererIndex + 1, selectedRendererName());
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
  buildGeneratedClickLibrary();
  drawScreen("ready");

  Serial.printf("click_lab: ready sounds=%u volume=%u\n",
                static_cast<unsigned>(rendererCount()), OUTPUT_VOLUME);
}

void loop() {
  M5.update();
  const uint32_t nowMs = millis();
  stopToneWhenDue(nowMs);

  if (M5.BtnA.wasPressed()) triggerSelected(nowMs);
  if (M5.BtnB.wasClicked()) selectNextRenderer();

  delay(1);
}
