#pragma once
#include "posture_ids.h"

inline const char *posIdToString(PosId id) {
  switch (id) {

  case PosId::TRUE_NEUTRAL:
    return "neutral";

  case PosId::LEAN_NEUTRAL_LEFT:
    return "lean_left_neutral";
  case PosId::LEAN_STRONG_LEFT:
    return "lean_left_strong";

  case PosId::LEAN_NEUTRAL_RIGHT:
    return "lean_right_neutral";
  case PosId::LEAN_STRONG_RIGHT:
    return "lean_right_strong";

  case PosId::SIT_FRONT:
    return "sit_front";

  case PosId::LEFT_LEG_CROSSED:
    return "leg_crossed_left";
  case PosId::RIGHT_LEG_CROSSED:
    return "leg_crossed_right";

  case PosId::NEUTRAL_NO_BACKREST:
    return "neutral_no_backrest";

  case PosId::LEAN_BACK:
    return "lean_back";

  case PosId::SLOUCH_UPPER_NEUTRAL:
    return "slouch_upper_neutral";
  case PosId::SLOUCH_LOWER_NEUTRAL:
    return "slouch_lower_neutral";

  case PosId::SLOUCH_UPPER_LEFT:
    return "slouch_upper_left";
  case PosId::SLOUCH_LOWER_LEFT:
    return "slouch_lower_left";

  case PosId::SLOUCH_UPPER_RIGHT:
    return "slouch_upper_right";
  case PosId::SLOUCH_LOWER_RIGHT:
    return "slouch_lower_right";

  case PosId::TWIST_LEFT:
    return "twist_left";
  case PosId::TWIST_RIGHT:
    return "twist_right";

  case PosId::UNOCCUPIED:
    return "unoccupied";

  case PosId::UNKNOWN:
  default:
    return "unknown";
  }
}