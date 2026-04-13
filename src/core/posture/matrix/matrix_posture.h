#pragma once
#include "core/posture/matrix/matrix_zones.h"
#include <Arduino.h>
#include <vector>

struct MatrixTags {
  bool occupied = false;
  // basic weight based directional tags
  bool weightLeft = false, weightRight = false, weightForward = false,
       weightBackward = false;
  bool neutral = false;

  // Advanced tags
  bool crossedLegLeft = false, crossedLegRight = false;
  bool sitOnLegLeft = false, sitOnLegRight = false;
  bool sittingFront = false;
};

class MatrixPosture {
public:
  static constexpr int INVALID_INT = -1;
  MatrixPosture(const int16_t *matrixPtr, uint8_t rows, uint8_t cols);

  // Compute Baseline
  void beginBaseline();
  void feedBaselineSample();
  void finalizeBaseline();
  bool hasBaseline() const { return _hasBaseline; }
  uint16_t getSize() const { return _rows * _cols; }

  const float *getBaseline() const { return _baseline; }
  const uint16_t *getCounts() const { return _count; }
  uint16_t getFrameCount() const { return _frames; }

  float deltaAt(uint16_t idx) const;

  const MatrixZone &getZone(MatrixZoneId id) const { return _zones.zone(id); }

  float zoneDeltaSum(MatrixZoneId id) const;
  float zoneDeltaMean(MatrixZoneId id) const;
  MatrixTags computeTags() const;

  float zoneRawSum(MatrixZoneId id) const;
  float baselineZoneSum(MatrixZoneId id) const;
  MatrixTags computeTagsBaseline() const;

private:
  const int16_t *_matrix;
  uint16_t _rows, _cols;
  uint16_t _frames = 0;
  bool _hasBaseline = false;

  float _baseline[49];
  uint16_t _count[49];
  MatrixZones _zones;
  float _baselineSeatRaw = 0.f;
};