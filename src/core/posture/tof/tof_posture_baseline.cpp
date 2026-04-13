#include "tof_posture.h"

void ToFPosture::beginBaseline() {
  _frames = 0;
  _hasBaseline = false;

  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    _backBaseline[i] = 0.f;
    _backCount[i] = 0;
  }
  for (uint8_t i = 0; i < TofData::HEAD_N; ++i) {
    _headBaseline[i] = 0.f;
    _headCount[i] = 0;
  }
  for (uint8_t i = 0; i < TofData::KNEE_N; ++i) {
    _kneeBaseline[i] = 0.f;
    _kneeCount[i] = 0;
  }
  for (uint8_t i = 0; i < TofData::BOTTOM_N; ++i) {
    _bottomBaseline[i] = 0.f;
    _bottomCount[i] = 0;
  }
}

void ToFPosture::feedBaselineSample() {
  if (!_tof)
    return;
  _frames++;

  auto updateMean = [](float &mean, uint16_t &n, int32_t v) {
    if (v == INVALID)
      return;
    ++n;
    mean = mean + (float(v) - mean) / float(n);
  };

  for (uint8_t i = 0; i < TofData::BACK_N; ++i)
    updateMean(_backBaseline[i], _backCount[i], _tof->back[i]);
  for (uint8_t i = 0; i < TofData::HEAD_N; ++i)
    updateMean(_headBaseline[i], _headCount[i], _tof->head[i]);
  for (uint8_t i = 0; i < TofData::KNEE_N; ++i)
    updateMean(_kneeBaseline[i], _kneeCount[i], _tof->knee[i]);
  for (uint8_t i = 0; i < TofData::BOTTOM_N; ++i)
    updateMean(_bottomBaseline[i], _bottomCount[i], _tof->bottom[i]);
}

void ToFPosture::finalizeBaseline() { _hasBaseline = true; }

// --- Delta pro Sensor ---

float ToFPosture::deltaBackAt(uint8_t idx) const {
  if (!_hasBaseline || !_tof)
    return 0.f;
  if (idx >= TofData::BACK_N)
    return 0.f;
  int32_t raw = _tof->back[idx];
  if (raw == INVALID)
    return 0.f;
  float base = _backBaseline[idx];
  return float(raw) - base; // >0: weiter weg als Baseline
}

float ToFPosture::deltaHeadAt(uint8_t idx) const {
  if (!_hasBaseline || !_tof)
    return 0.f;
  if (idx >= TofData::HEAD_N)
    return 0.f;
  int32_t raw = _tof->head[idx];
  if (raw == INVALID)
    return 0.f;
  float base = _headBaseline[idx];
  return float(raw) - base;
}

float ToFPosture::deltaKneeAt(uint8_t idx) const {
  if (!_hasBaseline || !_tof)
    return 0.f;
  if (idx >= TofData::KNEE_N)
    return 0.f;
  int32_t raw = _tof->knee[idx];
  if (raw == INVALID)
    return 0.f;
  float base = _kneeBaseline[idx];
  return float(raw) - base;
}

float ToFPosture::deltaBottomAt(uint8_t idx) const {
  if (!_hasBaseline || !_tof)
    return 0.f;
  if (idx >= TofData::BOTTOM_N)
    return 0.f;
  int32_t raw = _tof->bottom[idx];
  if (raw == INVALID)
    return 0.f;
  float base = _bottomBaseline[idx];
  return float(raw) - base;
}

// --- Aggregierte Mittelwerte ---

float ToFPosture::backDeltaMean() const {
  if (!_hasBaseline || !_tof)
    return 0.f;

  float sum = 0.f;
  uint8_t n = 0;
  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    if (_tof->back[i] != INVALID) {
      sum += deltaBackAt(i);
      ++n;
    }
  }
  return (n > 0) ? (sum / float(n)) : 0.f;
}

float ToFPosture::kneeDeltaMean() const {
  if (!_hasBaseline || !_tof)
    return 0.f;

  float sum = 0.f;
  uint8_t n = 0;
  for (uint8_t i = 0; i < TofData::KNEE_N; ++i) {
    if (_tof->knee[i] != INVALID) {
      sum += deltaKneeAt(i);
      ++n;
    }
  }
  return (n > 0) ? (sum / float(n)) : 0.f;
}

float ToFPosture::bottomDeltaMean() const {
  if (!_hasBaseline || !_tof)
    return 0.f;

  float sum = 0.f;
  uint8_t n = 0;
  for (uint8_t i = 0; i < TofData::BOTTOM_N; ++i) {
    if (_tof->bottom[i] != INVALID) {
      sum += deltaBottomAt(i);
      ++n;
    }
  }
  return (n > 0) ? (sum / float(n)) : 0.f;
}