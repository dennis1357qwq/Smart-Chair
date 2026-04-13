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

// Eine Zone = Menge von Indizes in der Matrix
struct MatrixZone {
  MatrixZoneId id;
  uint8_t indices[49]; // max. 7x7 = 49 Zellen
  uint8_t count = 0;   // wie viele sind wirklich belegt
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