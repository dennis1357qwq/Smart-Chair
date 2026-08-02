#include "tof_posture.h"

// --- Helper: Delta → DistanceLevel ---
// Annahme: Baseline ≈ "empfohlene neutral/angelehnte Haltung".
// d in mm: >0 = weiter weg als Baseline, <0 = näher an Lehne.

DistanceLevel ToFPosture::mapDeltaToLevel(float d, BackRow row) const {
  static constexpr float UPPER_CONTACT_EPS = 20.0f;
  static constexpr float UPPER_SLIGHT_MAX = 60.0f;
  static constexpr float UPPER_MEDIUM_MAX = 120.0f;

  static constexpr float MIDDLE_CONTACT_EPS = 15.0f;
  static constexpr float MIDDLE_SLIGHT_MAX = 60.0f;
  static constexpr float MIDDLE_MEDIUM_MAX = 120.0f;

  static constexpr float LOWER_CONTACT_EPS = 60.0f;
  static constexpr float LOWER_SLIGHT_MAX = 90.0f;
  static constexpr float LOWER_MEDIUM_MAX = 130.0f;

  float contactEps = 20.0f;
  float slightMax = 60.0f;
  float mediumMax = 120.0f;

  switch (row) {
  case BackRow::Upper:
    contactEps = UPPER_CONTACT_EPS;
    slightMax = UPPER_SLIGHT_MAX;
    mediumMax = UPPER_MEDIUM_MAX;
    break;
  case BackRow::Middle:
    contactEps = MIDDLE_CONTACT_EPS;
    slightMax = MIDDLE_SLIGHT_MAX;
    mediumMax = MIDDLE_MEDIUM_MAX;
    break;
  case BackRow::Lower:
    contactEps = LOWER_CONTACT_EPS;
    slightMax = LOWER_SLIGHT_MAX;
    mediumMax = LOWER_MEDIUM_MAX;
    break;
  }

  if (d < -contactEps) {
    return DistanceLevel::Contact;
  }
  if (d >= -contactEps && d <= contactEps) {
    return DistanceLevel::Contact;
  }
  if (d <= slightMax) {
    return DistanceLevel::SlightAway;
  }
  if (d <= mediumMax) {
    return DistanceLevel::MediumAway;
  }
  return DistanceLevel::FarAway;
}

// --- Helper: Diff links/rechts → AsymLevel ---
// diff = dLeft - dRight; >0: linke Seite weiter weg

AsymLevel ToFPosture::classifyAsym(float diff) const {
  float ad = fabs(diff);
  constexpr float ASYM_MILD = 15.0f;
  constexpr float ASYM_STRONG = 40.0f;

  if (ad < ASYM_MILD)
    return AsymLevel::None;
  if (ad < ASYM_STRONG)
    return AsymLevel::Mild;
  return AsymLevel::Strong;
}

int ToFPosture::distanceRank(DistanceLevel d) const {
  switch (d) {
  case DistanceLevel::Contact:
    return 0;
  case DistanceLevel::SlightAway:
    return 1;
  case DistanceLevel::MediumAway:
    return 2;
  case DistanceLevel::FarAway:
    return 3;
  case DistanceLevel::Unknown:
  default:
    return -1;
  }
}

// --- BackZoneLevels berechnen ---
// Annahme für BACK_N = 6 (bitte ggf. an reale Geometrie anpassen!)
// Index-Mapping (von oben nach unten):
//  0 = upper-left,   1 = upper-right
//  2 = middle-left,  3 = middle-right
//  4 = lower-left,   5 = lower-right

BackZoneLevels ToFPosture::computeBackLevels() const {
  BackZoneLevels lvl;
  if (!_hasBaseline || !_tof)
    return lvl;

  static constexpr float OFFSET_UPPER_MM = -5.0f;
  static constexpr float OFFSET_MIDDLE_MM = -15.0f;
  static constexpr float OFFSET_LOWER_MM = 0.0f;

  auto computeRow = [&](uint8_t idxLeft, uint8_t idxRight, BackRow row,
                        DistanceLevel &distLvl, AsymLevel &asymLvl,
                        bool &twistLeft, bool &twistRight) {
    twistLeft = false;
    twistRight = false;
    distLvl = DistanceLevel::Unknown;
    asymLvl = AsymLevel::None;

    bool hasL = (idxLeft < TofData::BACK_N) && (_tof->back[idxLeft] != INVALID);
    bool hasR =
        (idxRight < TofData::BACK_N) && (_tof->back[idxRight] != INVALID);

    if (!hasL && !hasR) {
      return;
    }

    float dL = hasL ? deltaBackAt(idxLeft) : 0.f;
    float dR = hasR ? deltaBackAt(idxRight) : 0.f;

    float mean = 0.f;
    if (hasL && hasR) {
      mean = 0.5f * (dL + dR);
    } else if (hasL) {
      mean = dL;
    } else {
      mean = dR;
    }

    float offset = 0.f;
    switch (row) {
    case BackRow::Upper:
      offset = OFFSET_UPPER_MM;
      break;
    case BackRow::Middle:
      offset = OFFSET_MIDDLE_MM;
      break;
    case BackRow::Lower:
      offset = OFFSET_LOWER_MM;
      break;
    }

    float meanCorr = mean + offset;

    distLvl = mapDeltaToLevel(meanCorr, row);

    // distLvl = mapDeltaToLevel(mean, row);

    if (hasL && hasR) {
      float diff = dL - dR;
      asymLvl = classifyAsym(diff);

      if (asymLvl != AsymLevel::None) {
        if (diff > 0) {
          twistRight = true;
        } else {
          twistLeft = true;
        }
      }
    }
  };

  computeRow(0, 3, BackRow::Upper, lvl.upper, lvl.upperAsym, lvl.upperTwistLeft,
             lvl.upperTwistRight);

  computeRow(2, 5, BackRow::Middle, lvl.middle, lvl.middleAsym,
             lvl.middleTwistLeft, lvl.middleTwistRight);

  computeRow(1, 4, BackRow::Lower, lvl.lower, lvl.lowerAsym, lvl.lowerTwistLeft,
             lvl.lowerTwistRight);

  lvl.valid = true;
  return lvl;
}