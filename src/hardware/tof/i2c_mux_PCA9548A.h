#pragma once
#include <Arduino.h>
#include <Wire.h>

struct PCA9548A {
	uint8_t addr;
	explicit PCA9548A(uint8_t a = 0x70) : addr(a) {}

	inline void init() {}; // Wire.begin is handeled globaly right now.

	inline void select(uint8_t channel) const {
		Wire.beginTransmission(addr);
		Wire.write(uint8_t(1) << (channel & 0x07));
		Wire.endTransmission();
		delay(2);
	}

	inline void disableAll() const {
		Wire.beginTransmission(addr);
		Wire.write(0x00); // alle 8 Kanäle aus
		Wire.endTransmission();
		delay(2);
  }
};