#include "tof_posture.h"

ToFPosture::ToFPosture(const TofData *tofPtr) : _tof(tofPtr) {
  beginBaseline();
}

void ToFPosture::beginBaseline() {
  _frames = 0;
  _hasBaseline = false;

  // alles resetten
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

  // Helfer-Lambda für gleitenden Mittelwert
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
  return float(raw) - base; // >0: weiter weg als Baseline, <0: näher dran
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

// --- Helper: Delta → DistanceLevel ---
// Annahme: Baseline ≈ "empfohlene neutral/angelehnte Haltung".
// d in mm: >0 = weiter weg als Baseline, <0 = näher an Lehne.

DistanceLevel ToFPosture::mapDeltaToLevel(float d, BackRow row) const {
  // Upper: eher „präzise“
  static constexpr float UPPER_CONTACT_EPS = 20.0f;
  static constexpr float UPPER_SLIGHT_MAX = 60.0f;
  static constexpr float UPPER_MEDIUM_MAX = 120.0f;

  // Middle: erstmal wie upper, kannst du später separat tweaken
  static constexpr float MIDDLE_CONTACT_EPS = 20.0f;
  static constexpr float MIDDLE_SLIGHT_MAX = 60.0f;
  static constexpr float MIDDLE_MEDIUM_MAX = 120.0f;

  // Lower: etwas großzügiger, um deine False-Negatives zu killen
  static constexpr float LOWER_CONTACT_EPS = 30.0f; // statt 20
  static constexpr float LOWER_SLIGHT_MAX = 70.0f;  // leicht größer
  static constexpr float LOWER_MEDIUM_MAX = 130.0f; // leicht größer

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
    // Noch näher an Lehne als Baseline → trotzdem "Kontakt".
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
  constexpr float ASYM_MILD = 15.0f;   // ~1.5 cm
  constexpr float ASYM_STRONG = 40.0f; // ~4 cm

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
      // keine gültigen Sensoren → Unknown/None
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

    distLvl = mapDeltaToLevel(mean, row);

    // Asymmetrie nur sinnvoll, wenn beide Seiten gültig
    if (hasL && hasR) {
      float diff = dL - dR; // >0: linke Seite weiter weg
      asymLvl = classifyAsym(diff);

      if (asymLvl != AsymLevel::None) {
        if (diff > 0) {
          twistRight = true; // Körper nach rechts gedreht
        } else {
          twistLeft = true; // Körper nach links gedreht
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

// --- Tags berechnen ---

ToFTags ToFPosture::computeTags() const {
  ToFTags tags;

  if (!_hasBaseline || !_tof) {
    return tags;
  }

  // 1) Back-Präsenz (backInRange)
  uint8_t validBack = 0;
  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    int32_t v = _tof->back[i];
    if (v != INVALID && v > 0 && v < 500) { // 0..500 mm als sinnvoller Bereich
      ++validBack;
    }
  }
  if (validBack >= 2) {
    tags.backInRange = true;
  }

  // 2) Zonen-Level (oben / mitte / unten, inkl. Asymmetrien)
  BackZoneLevels lvl = computeBackLevels();
  if (!lvl.valid) {
    return tags; // keine sinnvolle Rückeninfo
  }

  // 3) Globale Torso-Vor-/Zurück-Neigung
  float dBack = backDeltaMean(); // in mm

  constexpr float TORSO_FWD_MM = 30.0f;  // ~3 cm weiter weg
  constexpr float TORSO_BWD_MM = -30.0f; // ~3 cm näher an Lehne

  if (tags.backInRange) {
    if (dBack > TORSO_FWD_MM)
      tags.torsoForward = true;
    if (dBack < TORSO_BWD_MM)
      tags.torsoBackward = true;
  }

  // 4) Kontakt-Tags (jetzt mit middle)
  tags.contactUpper = (lvl.upper == DistanceLevel::Contact);
  tags.contactMiddle = (lvl.middle == DistanceLevel::Contact);
  tags.contactLower = (lvl.lower == DistanceLevel::Contact);
  // Kontakt, wenn jede Zone "anliegt"
  tags.contact = (tags.contactUpper && tags.contactMiddle && tags.contactLower);

  // 5) Distanz-Tags (upper / middle / lower)
  auto setAwayFlags = [](DistanceLevel dl, bool &slight, bool &medium,
                         bool &far) {
    slight = medium = far = false;
    switch (dl) {
    case DistanceLevel::SlightAway:
      slight = true;
      break;
    case DistanceLevel::MediumAway:
      medium = true;
      break;
    case DistanceLevel::FarAway:
      far = true;
      break;
    default:
      break;
    }
  };

  setAwayFlags(lvl.upper, tags.upperSlightAway, tags.upperMediumAway,
               tags.upperFarAway);

  setAwayFlags(lvl.middle, tags.middleSlightAway, tags.middleMediumAway,
               tags.middleFarAway);

  setAwayFlags(lvl.lower, tags.lowerSlightAway, tags.lowerMediumAway,
               tags.lowerFarAway);

  // 6) Asymmetrie-Tags + Drehrichtung (upper / middle / lower)

  if (lvl.upperAsym != AsymLevel::None) {
    tags.upperAsymmetry = true;
    if (lvl.upperTwistLeft)
      tags.upperTwistLeft = true;
    if (lvl.upperTwistRight)
      tags.upperTwistRight = true;
  }

  if (lvl.middleAsym != AsymLevel::None) {
    tags.middleAsymmetry = true;
    if (lvl.middleTwistLeft)
      tags.middleTwistLeft = true;
    if (lvl.middleTwistRight)
      tags.middleTwistRight = true;
  }

  if (lvl.lowerAsym != AsymLevel::None) {
    tags.lowerAsymmetry = true;
    if (lvl.lowerTwistLeft)
      tags.lowerTwistLeft = true;
    if (lvl.lowerTwistRight)
      tags.lowerTwistRight = true;
  }

  // 7) NEUTRAL: Rücken relativ "gerade"
  //  - alle bekannten Zonen haben ähnliche DistanceLevels (maxRank - minRank <=
  //  1)
  //  - keine Asymmetrie in upper/middle/lower

  int ranks[3];
  ranks[0] = distanceRank(lvl.upper);
  ranks[1] = distanceRank(lvl.middle);
  ranks[2] = distanceRank(lvl.lower);

  int minRank = 100;
  int maxRank = -100;
  bool anyRank = false;

  for (int i = 0; i < 3; ++i) {
    if (ranks[i] >= 0) {
      anyRank = true;
      if (ranks[i] < minRank)
        minRank = ranks[i];
      if (ranks[i] > maxRank)
        maxRank = ranks[i];
    }
  }

  bool noAsym = (lvl.upperAsym == AsymLevel::None) &&
                (lvl.middleAsym == AsymLevel::None) &&
                (lvl.lowerAsym == AsymLevel::None);

  if (tags.backInRange && anyRank && noAsym) {
    if ((maxRank - minRank) <= 1) {
      tags.neutral = true;
    }
  }

  return tags;
}