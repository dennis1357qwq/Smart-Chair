#pragma once
#include <stdint.h>

enum class PosId : int32_t {
	UNKNOWN = -1,
	NEUTRAL = 0,
	LEAN_LEFT = 1,
	LEAN_RIGHT = 2,
	SIT_FRONT = 3,
	LEFT_LEG_CROSSED = 4,
	RIGHT_LEG_CROSSED = 5,
	NEUTRAL_NO_BACKREST = 6,
};