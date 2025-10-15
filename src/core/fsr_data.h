#pragma once
#include <stdint.h>

enum FsrId : uint8_t { FSR_LORD=0, FSR_LEFT=1, FSR_RIGHT=2, FSR_COUNT=3 };

struct FsrData {
  int32_t values[FSR_COUNT] = { -1, -1, -1 };
};