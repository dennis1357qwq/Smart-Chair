#pragma once
#include "posture_ids.h"
inline const char *posIdToString(PosId id) {
  switch (id) {
  case PosId::NEUTRAL:
    return "neutral";
  default:
    return "unknown";
  }
}