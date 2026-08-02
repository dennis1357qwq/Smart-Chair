#include "tof_posture.h"

// --- Tags berechnen ---

ToFTags ToFPosture::computeTags() const {
  ToFTags tags;

  if (!_hasBaseline || !_tof) {
    return tags;
  }

  // 1) Back-Präsenz
  uint8_t validBack = 0;
  for (uint8_t i = 0; i < TofData::BACK_N; ++i) {
    int32_t v = _tof->back[i];
    if (v != INVALID && v > 0 && v < 500) { // 0..500 mm
      ++validBack;
    }
  }
  if (validBack >= 2) {
    tags.backInRange = true;
  }

  // 2) Zonen-Level
  BackZoneLevels lvl = computeBackLevels();
  if (!lvl.valid) {
    return tags;
  }

  // 3) Torso-Neigung
  float dBack = backDeltaMean();
  constexpr float TORSO_FWD_MM = 30.0f;
  constexpr float TORSO_BWD_MM = -30.0f;

  if (tags.backInRange) {
    if (dBack > TORSO_FWD_MM)
      tags.torsoForward = true;
    if (dBack < TORSO_BWD_MM)
      tags.torsoBackward = true;
  }

  // 4) Kontakt
  tags.contactUpper = (lvl.upper == DistanceLevel::Contact);
  tags.contactMiddle = (lvl.middle == DistanceLevel::Contact);
  tags.contactLower = (lvl.lower == DistanceLevel::Contact);
  tags.contact = (tags.contactUpper && tags.contactMiddle && tags.contactLower);

  // 5) Distanz-Tags
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

  // 6) Asymmetrie-Tags
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

  // 7) Neutral
  int ranks[3] = {distanceRank(lvl.upper), distanceRank(lvl.middle),
                  distanceRank(lvl.lower)};

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