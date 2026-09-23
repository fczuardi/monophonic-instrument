#pragma once

#include <cstdint>

enum class AudioIdlePolicy : uint8_t {
  StopWhenIdle,
  KeepAlive,
};
