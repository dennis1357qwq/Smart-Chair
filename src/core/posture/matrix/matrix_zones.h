#pragma once
#include <Arduino.h>

enum class MatrixZoneId : uint8_t {
  LeftHalf,
  RightHalf,
  FrontHalf,
  BackHalf,

  FrontLeft,
  FrontRight,
  BackLeft,
  BackRight
};

// Zone = Amount of Indices in the Matrix
struct MatrixZone {
  MatrixZoneId id;
  uint8_t indices[49]; // 7 x 7 cells
  uint8_t count = 0;
};

class MatrixZones {
public:
  static constexpr uint8_t MAX_ZONES = 8;

  MatrixZones(uint8_t rows, uint8_t cols);

  const MatrixZone &zone(MatrixZoneId id) const;
  uint8_t zoneCount() const { return _zoneCount; }

private:
  uint8_t _rows;
  uint8_t _cols;
  MatrixZone _zones[MAX_ZONES];
  uint8_t _zoneCount = 0;

  void buildZones();
};