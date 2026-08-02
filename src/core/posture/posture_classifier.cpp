// posture_classifier.cpp
#include "core/posture/posture_classifier.h"

// MatrixTags:
//   - occupied, neutral, weightLeft, weightRight, weightForward, weightBackward
// BackMetaTags:
//   - inRange, backNeutral, backNear/backMid/backFar
//   - slouchShoulder, slouchTorso (torso used to represent "step slouch" when
//   combined)

PosId PostureClassifier::classify(const MatrixTags &m,
                                  const BackMetaTags &b) const {

  if (!m.occupied) {
    return PosId::UNOCCUPIED;
  }

  // Convenience flags
  const bool anyLeft = m.weightLeft;
  const bool anyRight = m.weightRight;
  const bool forward = m.weightForward;
  const bool backward = m.weightBackward;

  const bool backContact = b.backNear;
  const bool shoulderSlouch = b.slouchShoulder;
  const bool torsoSlouch = b.slouchTorso;

  const bool stepSlouch = shoulderSlouch && torsoSlouch;
  const bool anySlouch = shoulderSlouch || torsoSlouch;

  if (shoulderSlouch) {
    if (anyLeft) {
      return stepSlouch ? PosId::SLOUCH_LOWER_LEFT : PosId::SLOUCH_UPPER_LEFT;
    }
    if (anyRight) {
      return stepSlouch ? PosId::SLOUCH_LOWER_RIGHT : PosId::SLOUCH_UPPER_RIGHT;
    }

    if (m.neutral || forward) {
      return stepSlouch ? PosId::SLOUCH_LOWER_NEUTRAL
                        : PosId::SLOUCH_UPPER_NEUTRAL;
    }

    return stepSlouch ? PosId::SLOUCH_LOWER_NEUTRAL
                      : PosId::SLOUCH_UPPER_NEUTRAL;
  }

  if (backward && backContact && !anySlouch) {
    return PosId::LEAN_BACK;
  }
  if (forward && !backContact) {
    return PosId::SIT_FRONT;
  }

  if (anyLeft) {
    return m.neutral ? PosId::LEAN_NEUTRAL_LEFT : PosId::LEAN_STRONG_LEFT;
  }
  if (anyRight) {
    return m.neutral ? PosId::LEAN_NEUTRAL_RIGHT : PosId::LEAN_STRONG_RIGHT;
  }

  if (m.neutral && !backContact) {
    return PosId::NEUTRAL_NO_BACKREST;
  }

  if (m.neutral) {

    if (b.backNeutral) {
      return PosId::TRUE_NEUTRAL;
    }

    return PosId::TRUE_NEUTRAL;
  }

  return PosId::UNKNOWN;
}