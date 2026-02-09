#pragma once
#include <stdint.h>

enum class PosId : int32_t {
  UNKNOWN = -1,

  TRUE_NEUTRAL = 0,       // Neutral Matrix, Neutral Back
  LEAN_NEUTRAL_LEFT = 1,  // Neutral + Left Matrix, Neutral Back
  LEAN_STRONG_LEFT = 2,   // Left Matrix, Neutral Back
  LEAN_NEUTRAL_RIGHT = 3, // Neutral + Right Matrix, Neutral Back
  LEAN_STRONG_RIGHT = 4,  // Right Matrix, Neutral Back

  SIT_FRONT = 5, // Matrix front, No Contact Back + no slouch shoulder

  LEFT_LEG_CROSSED = 6,  // Matrix Zone detection - TBD
  RIGHT_LEG_CROSSED = 7, // Matrix Zone detection - TBD

  NEUTRAL_NO_BACKREST = 8, // Neutral Matrix, No contact Back, No Slouched Back

  LEAN_BACK = 9,             // Back Matrix + Contact Back + No slouch
  SLOUCH_UPPER_NEUTRAL = 10, // Matrix Neutral or Forward + Shoulder Slouch Back
  SLOUCH_LOWER_NEUTRAL =
      11, // Matrix Neutral or Forward + Shoulder Slouch + Torso Slouch Back
  SLOUCH_UPPER_LEFT = 12, // Matrix any left + Shoulder Slouch Back
  SLOUCH_LOWER_LEFT =
      13, // Matrix any left + Shoulder Slouch + Torso Slouch Back
  SLOUCH_UPPER_RIGHT = 14, // Matrix any Right + Shoulder Slouch Back
  SLOUCH_LOWER_RIGHT =
      15,           // Matrix any Right + Shoulder Slouch + Torso Slouch Back
  TWIST_LEFT = 16,  // Asym Back - TBD
  TWIST_RIGHT = 17, // Asym Back - TBD

  UNOCCUPIED = 18 // No Matrix, No Back
};