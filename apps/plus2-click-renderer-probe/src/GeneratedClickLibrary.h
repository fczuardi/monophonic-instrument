#pragma once

#include <cstddef>
#include <cstdint>

#include "PcmS8Sample.h"

enum class GeneratedClickId : uint8_t {
  LowKnock,
  WoodBlock,
  BrightClave,
  RimBlend,
  DryImpulse,
  NoiseTap,
  BrushNoise,
  DownSweep,
  Count,
};

void buildGeneratedClickLibrary();
size_t generatedClickCount();
const char* generatedClickName(GeneratedClickId id);
const PcmS8Sample& generatedClickSample(GeneratedClickId id);
