#pragma once
#include <stdint.h>

struct MatrixData {
  static constexpr uint8_t ROWS = 7;
  static constexpr uint8_t COLS = 7;

  // flat Array (Row-major). -1 = invalid
  // acces via .values[r * MatrixData::COLS + c]
  int16_t values[ROWS * COLS];
};