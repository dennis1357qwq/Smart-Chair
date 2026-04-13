#include "matrix_posture.h"
#include <Arduino.h>

MatrixPosture::MatrixPosture(const int16_t *matrixPtr, uint8_t rows,
                             uint8_t cols)
    : _matrix(matrixPtr), _rows(rows), _cols(cols), _zones(rows, cols) {
  beginBaseline();
}

void MatrixPosture::beginBaseline() {
  const uint16_t N = _rows * _cols;
  _frames = 0;
  _hasBaseline = false;
  for (uint16_t i = 0; i < N; ++i) {
    _baseline[i] = 0.f;
    _count[i] = 0;
  }
}

void MatrixPosture::feedBaselineSample() {
  if (!_matrix)
    return;
  const uint16_t N = _rows * _cols;
  _frames++;
  for (uint16_t i = 0; i < N; ++i) {
    int v = _matrix[i];
    if (v == INVALID_INT)
      continue;
    uint16_t &n = _count[i];
    n++;
    float b = _baseline[i];
    _baseline[i] = b + (float(v) - b) / float(n);
  }
}

void MatrixPosture::finalizeBaseline() {
  _hasBaseline = true;
  _baselineSeatRaw = 0.f;
  _baselineSeatRaw += baselineZoneSum(MatrixZoneId::LeftHalf);
  _baselineSeatRaw += baselineZoneSum(MatrixZoneId::RightHalf);
}

float MatrixPosture::baselineZoneSum(MatrixZoneId id) const {
  const MatrixZone &z = _zones.zone(id);
  float sum = 0.f;
  for (uint8_t i = 0; i < z.count; ++i) {
    uint8_t idx = z.indices[i];
    // hier explizit die Baseline nehmen, NICHT _matrix
    sum += _baseline[idx];
  }
  return sum;
}

float MatrixPosture::deltaAt(uint16_t idx) const {
  if (!_hasBaseline)
    return 0.f;
  if (!_matrix)
    return 0.f;

  int raw = _matrix[idx];
  if (raw == INVALID_INT)
    return 0.f;

  float base = _baseline[idx];
  return float(raw) - base;
}

// aggregates matrix values over a zone
float MatrixPosture::zoneDeltaSum(MatrixZoneId id) const {
  const MatrixZone &z = _zones.zone(id);
  if (!_hasBaseline || !_matrix)
    return 0.f;

  static constexpr float CELL_EPS =
      15.0f; // unter ~15 ADC-Punkte: als Rauschen ignorieren

  float sum = 0.f;
  for (uint8_t i = 0; i < z.count; ++i) {
    uint8_t idx = z.indices[i];
    float d = deltaAt(idx);

    if (d > -CELL_EPS && d < CELL_EPS)
      continue;

    if (d > 0.f) {
      sum += d;
    }
  }
  return sum;
}

// means values over a zone
float MatrixPosture::zoneDeltaMean(MatrixZoneId id) const {
  const MatrixZone &z = _zones.zone(id);
  if (z.count == 0)
    return 0.f;
  return zoneDeltaSum(id) / float(z.count);
}

float MatrixPosture::zoneRawSum(MatrixZoneId id) const {
  const MatrixZone &z = _zones.zone(id);
  if (!_matrix)
    return 0.f;

  float sum = 0.f;
  for (uint8_t i = 0; i < z.count; ++i) {
    uint8_t idx = z.indices[i];
    int16_t v = _matrix[idx];
    if (v == INVALID_INT)
      continue;
    sum += float(v);
  }
  return sum;
}

MatrixTags MatrixPosture::computeTags() const {
  MatrixTags tags;

  if (!_matrix) {
    return tags;
  }

  // 1) OCCUPIED über Delta (Baseline) – UNVERÄNDERT
  float fullDelta = 0.f;
  fullDelta += zoneDeltaSum(MatrixZoneId::LeftHalf);
  fullDelta += zoneDeltaSum(MatrixZoneId::RightHalf);
  fullDelta += zoneDeltaSum(MatrixZoneId::FrontHalf);
  fullDelta += zoneDeltaSum(MatrixZoneId::BackHalf);

  static constexpr float OCC_MIN = 8000.0f;
  if (fullDelta > OCC_MIN) {
    tags.occupied = true;
  } else {
    tags.occupied = false;
    tags.neutral = false;
    return tags;
  }

  tags.neutral = true;

  // 2) LEAN über ROHWERTE (nicht Delta), mit L/R-Korrektur – leicht erweitert
  float leftRaw = zoneRawSum(MatrixZoneId::RightHalf); // physikalisch links
  float rightRaw = zoneRawSum(MatrixZoneId::LeftHalf); // physikalisch rechts
  float frontRaw = zoneRawSum(MatrixZoneId::FrontHalf);
  float backRaw = zoneRawSum(MatrixZoneId::BackHalf);

  float lrTotal = leftRaw + rightRaw;
  float fbTotal = frontRaw + backRaw;

  // Mindest-Gesamtlast, damit es nicht bei ganz leichten Berührungen triggert
  static constexpr float MIN_TOTAL = 15000.0f;

  // Zwei Lean-Schwellen:
  // - mild: Lean + neutral = true
  // - strong: Lean = true, neutral = false
  static constexpr float LEAN_EPS_MILD = 0.08f;   // ~8% Unterschied
  static constexpr float LEAN_EPS_STRONG = 0.14f; // ~18% Unterschied

  bool anyLeanLR = false;
  bool anyLeanFB = false;

  // --- Links / Rechts ---
  if (lrTotal > MIN_TOTAL) {
    float leftShare = leftRaw / lrTotal;
    float rightShare = rightRaw / lrTotal;
    float diffLR = leftShare - rightShare; // >0 = mehr links

    if (diffLR > LEAN_EPS_MILD) {
      tags.weightLeft = true;
      anyLeanLR = true;
      if (diffLR >= LEAN_EPS_STRONG) {
        tags.neutral = false; // starker Lean → nicht mehr neutral
      }
    } else if (diffLR < -LEAN_EPS_MILD) {
      tags.weightRight = true;
      anyLeanLR = true;
      if (-diffLR >= LEAN_EPS_STRONG) {
        tags.neutral = false;
      }
    }
  }

  // --- Vorne / Hinten ---
  if (fbTotal > MIN_TOTAL) {
    float frontShare = frontRaw / fbTotal;
    float backShare = backRaw / fbTotal;
    float diffFB = frontShare - backShare; // >0 = mehr vorne

    if (diffFB > LEAN_EPS_MILD) {
      tags.weightForward = true;
      anyLeanFB = true;
      if (diffFB >= LEAN_EPS_STRONG) {
        tags.neutral = false;
      }
    } else if (diffFB < -LEAN_EPS_MILD) {
      tags.weightBackward = true;
      anyLeanFB = true;
      if (-diffFB >= LEAN_EPS_STRONG) {
        tags.neutral = false;
      }
    }
  }

  // Wenn GAR kein Lean detektiert wurde → sicher neutral
  if (!anyLeanLR && !anyLeanFB) {
    tags.neutral = true;
  }
  // Wenn milde Leans: neutral bleibt true (weil oben nicht auf false gesetzt).
  // Wenn starke Leans: neutral wurde oben auf false gesetzt.

  return tags;
}

MatrixTags MatrixPosture::computeTagsBaseline() const {
  MatrixTags tags;

  if (!_matrix || !_hasBaseline) {
    // Ohne Daten oder Baseline nichts Sinnvolles
    return tags;
  }

  // 1) OCCUPIED: Raw-Sitzlast relativ zur Baseline
  float seatRawNow = 0.f;
  seatRawNow += zoneRawSum(MatrixZoneId::LeftHalf);
  seatRawNow += zoneRawSum(MatrixZoneId::RightHalf);

  // Sicherheitsminimum, falls Baseline mal sehr klein ist (z.B. Lab-Test)
  static constexpr float OCC_MIN_ABS = 3000.0f;
  // Faktor: "mindestens x% der Baseline-Sitzlast"
  static constexpr float OCC_FACTOR = 0.5f; // 50% – kannst du später tweaken

  float occThreshold = _baselineSeatRaw * OCC_FACTOR;
  if (occThreshold < OCC_MIN_ABS)
    occThreshold = OCC_MIN_ABS;

  if (seatRawNow > occThreshold) {
    tags.occupied = true;
  } else {
    // Niemand "richtig" drauf → fertig
    tags.occupied = false;
    tags.neutral = false;
    return tags;
  }

  // Ab hier: jemand sitzt drauf.
  // Default: erstmal neutral = true, starke Leans schalten es ggf. aus.
  tags.neutral = true;

  // 2) LEAN – jetzt über DELTAS statt Raw
  //    Achtung: LeftHalf/RightHalf sind im Code vertauscht zur physischen
  //    Seite, darum wie gehabt: RightHalf = physikalisch links, LeftHalf =
  //    physikalisch rechts.
  float leftDelta = zoneDeltaSum(MatrixZoneId::RightHalf); // physikalisch links
  float rightDelta =
      zoneDeltaSum(MatrixZoneId::LeftHalf); // physikalisch rechts
  float frontDelta = zoneDeltaSum(MatrixZoneId::FrontHalf);
  float backDelta = zoneDeltaSum(MatrixZoneId::BackHalf);

  float lrTotalDelta = leftDelta + rightDelta;
  float fbTotalDelta = frontDelta + backDelta;

  // Mindest-Gesamt-Delta, damit kleinste Bewegungen nicht gleich als Lean
  // zählen
  static constexpr float DELTA_MIN_TOTAL = 4500.0f;

  // Zwei Lean-Schwellen:
  // - mild: Lean + neutral = true
  // - strong: Lean = true, neutral = false
  static constexpr float LEAN_EPS_MILD_LR = 0.30f;   // ~8% Unterschied
  static constexpr float LEAN_EPS_STRONG_LR = 0.80f; // ~14% Unterschied

  static constexpr float LEAN_EPS_MILD_FB = 0.55f;   // 12%
  static constexpr float LEAN_EPS_STRONG_FB = 0.80f; // 22%

  bool anyLeanLR = false;
  bool anyLeanFB = false;

  // --- Links / Rechts über Deltas ---
  if (lrTotalDelta > DELTA_MIN_TOTAL) {
    float leftShare = (lrTotalDelta > 0.f) ? leftDelta / lrTotalDelta : 0.f;
    float rightShare = (lrTotalDelta > 0.f) ? rightDelta / lrTotalDelta : 0.f;
    float diffLR = leftShare - rightShare; // >0 = mehr links

    if (diffLR > LEAN_EPS_MILD_LR) {
      tags.weightLeft = true;
      anyLeanLR = true;
      if (diffLR >= LEAN_EPS_STRONG_LR) {
        tags.neutral = false; // starker Lean → nicht mehr neutral
      }
    } else if (diffLR < -LEAN_EPS_MILD_LR) {
      tags.weightRight = true;
      anyLeanLR = true;
      if (-diffLR >= LEAN_EPS_STRONG_LR) {
        tags.neutral = false;
      }
    }
  }

  // --- Vorne / Hinten über Deltas ---
  if (fbTotalDelta > DELTA_MIN_TOTAL) {
    float frontShare = (fbTotalDelta > 0.f) ? frontDelta / fbTotalDelta : 0.f;
    float backShare = (fbTotalDelta > 0.f) ? backDelta / fbTotalDelta : 0.f;
    float diffFB = frontShare - backShare; // >0 = mehr vorne

    if (diffFB > LEAN_EPS_MILD_FB) {
      tags.weightForward = true;
      anyLeanFB = true;
      if (diffFB >= LEAN_EPS_STRONG_FB) {
        tags.neutral = false;
      }
    } else if (diffFB < -LEAN_EPS_MILD_FB) {
      tags.weightBackward = true;
      anyLeanFB = true;
      if (-diffFB >= LEAN_EPS_STRONG_FB) {
        tags.neutral = false;
      }
    }
  }

  // Wenn GAR kein Lean detektiert wurde → sicher neutral
  if (!anyLeanLR && !anyLeanFB) {
    tags.neutral = true;
  }
  // Milde Leans lassen neutral an; starke Leans haben es oben bereits
  // ausgeschaltet.

  return tags;
}