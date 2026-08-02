#pragma once
#include <stdint.h>

struct TofData {
  static constexpr uint8_t BACK_N   = 6;
  static constexpr uint8_t HEAD_N   = 1;
  static constexpr uint8_t KNEE_N   = 2;
  static constexpr uint8_t BOTTOM_N = 2;

  // distance in mm; -1 = invalid
  int32_t back[BACK_N]     = { -1, -1, -1, -1, -1, -1 };
  int32_t head[HEAD_N]     = { -1 };
  int32_t knee[KNEE_N]     = { -1, -1 };
  int32_t bottom[BOTTOM_N] = { -1, -1 };
};