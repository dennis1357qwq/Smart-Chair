#include "matrix_zones.h"

MatrixZones::MatrixZones(uint8_t rows, uint8_t cols)
: _rows(rows), _cols(cols) {
  buildZones();
}

const MatrixZone& MatrixZones::zone(MatrixZoneId id) const {
  // Einfacher Linear-Suchlauf – wenig Zonen, daher ok
  for (uint8_t i = 0; i < _zoneCount; ++i) {
    if (_zones[i].id == id) return _zones[i];
  }
  // Fallback: erste Zone zurückgeben (sollte im Normalfall nicht passieren)
  return _zones[0];
}

void MatrixZones::buildZones() {
  _zoneCount = 0;
  const uint8_t midRow = _rows / 2; // 7 -> 3
  const uint8_t midCol = _cols / 2; // 7 -> 3

  auto& leftHalf   = _zones[_zoneCount++]; leftHalf.id   = MatrixZoneId::LeftHalf;
  auto& rightHalf  = _zones[_zoneCount++]; rightHalf.id  = MatrixZoneId::RightHalf;
  auto& frontHalf  = _zones[_zoneCount++]; frontHalf.id  = MatrixZoneId::FrontHalf;
  auto& backHalf   = _zones[_zoneCount++]; backHalf.id   = MatrixZoneId::BackHalf;

  auto& frontLeft  = _zones[_zoneCount++]; frontLeft.id  = MatrixZoneId::FrontLeft;
  auto& frontRight = _zones[_zoneCount++]; frontRight.id = MatrixZoneId::FrontRight;
  auto& backLeft   = _zones[_zoneCount++]; backLeft.id   = MatrixZoneId::BackLeft;
  auto& backRight  = _zones[_zoneCount++]; backRight.id  = MatrixZoneId::BackRight;

  // Alle Indizes einmal durchgehen und je nach row/col Zonen zuordnen
  for (uint8_t r = 0; r < _rows; ++r) {
    for (uint8_t c = 0; c < _cols; ++c) {
      const uint8_t idx = r * _cols + c;

      const bool isFront = (r <= midRow);      // überlappende Halben
      const bool isBack  = (r >= midRow);
      const bool isLeft  = (c <= midCol);
      const bool isRight = (c >= midCol);

      // Halbräume
      if (isLeft  && leftHalf.count  < 49) leftHalf.indices[leftHalf.count++]   = idx;
      if (isRight && rightHalf.count < 49) rightHalf.indices[rightHalf.count++] = idx;
      if (isFront && frontHalf.count < 49) frontHalf.indices[frontHalf.count++] = idx;
      if (isBack  && backHalf.count  < 49) backHalf.indices[backHalf.count++]  = idx;

      // Quadranten: Mitte explizit ausgespart
      const bool isFrontSide = (r < midRow);
      const bool isBackSide  = (r > midRow);
      const bool isLeftSide  = (c < midCol);
      const bool isRightSide = (c > midCol);

      if (isFrontSide && isLeftSide  && frontLeft.count  < 49)
        frontLeft.indices[frontLeft.count++] = idx;

      if (isFrontSide && isRightSide && frontRight.count < 49)
        frontRight.indices[frontRight.count++] = idx;

      if (isBackSide  && isLeftSide  && backLeft.count   < 49)
        backLeft.indices[backLeft.count++] = idx;

      if (isBackSide  && isRightSide && backRight.count  < 49)
        backRight.indices[backRight.count++] = idx;
    }
  }
}