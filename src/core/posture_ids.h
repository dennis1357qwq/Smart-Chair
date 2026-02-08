#pragma once
#include <stdint.h>

enum class PosId : int32_t {
  UNKNOWN = -1,

  NEUTRAL = 0, // „gut“ angelehnt oder sehr gleichmäßige Rückenposition
  LEAN_NEUTRAL_LEFT = 1,
  LEAN_STRONG_LEFT = 2,
  LEAN_NEUTRAL_RIGHT = 3,
  LEAN_STRONG_RIGHT = 4,

  // SIT_FRONT kannst du als „Lean Forward“ interpretieren
  SIT_FRONT = 5, // historisch: nach vorne verlagert; alias of LEAN_FORWARD

  LEFT_LEG_CROSSED = 6,
  RIGHT_LEG_CROSSED = 7,

  NEUTRAL_NO_BACKREST = 8, // auf Sitzfläche, aber no contact

  // NEU:
  LEAN_BACK = 9,
  SLOUCH_UPPER = 10,
  SLOUCH_LOWER = 11,
  TWIST_LEFT = 12,
  TWIST_RIGHT = 13,

  OFF_CHAIR = 14 // niemand sitzt – optional, aber praktisch
};