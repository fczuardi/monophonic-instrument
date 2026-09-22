#include <Arduino.h>
#include <M5Unified.h>

#include "M5BuzzerToneOutput.h"
#include "M5CoreGrayToneOutput.h"
#include "MonophonicInstrument.h"
#include "MonophonicInstrumentSink.h"
#include "PcmS8Sample.h"

MonophonicInstrument instrument;
M5BuzzerToneOutput output;
M5CoreGrayToneOutput coreGrayOutput;
MonophonicInstrumentSink sink(instrument, output);
const int8_t clickData[] = {0, 48, -32, 12, 0};
const PcmS8Sample click = {
    clickData,
    sizeof(clickData) / sizeof(clickData[0]),
    16000,
};

void setup() {
  output.setWaveform(instrument.waveform());
  output.setVelocityVolumeRange({64, 128});
  coreGrayOutput.setWaveform(ToneWaveform::Square32);
  coreGrayOutput.setVelocityVolumeRange({1, 35});
  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 60, 0});
  output.playSample(click);
  output.stop();
}

void loop() {
}
