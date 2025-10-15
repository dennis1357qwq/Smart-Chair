#pragma once
#include "posture_ids.h"
inline const char* posIdToString(PosId id) {
  switch (id) {
    case PosId::NEUTRAL:return "neutral";
    case PosId::LEAN_LEFT:return "lean_left";
    case PosId::LEAN_RIGHT:return "lean_right";
    case PosId::SIT_FRONT:return "sitting_front";
    case PosId::NEUTRAL_NO_BACKREST:return "neutral_no_backrest";
    default:return "unknown";
  }
}