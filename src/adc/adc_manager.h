#pragma once
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

struct Adc {
	Adafruit_ADS1115 ads;

	bool initADC() {
		if (!ads.begin()) {
			Serial.println("Error: ADS1115 not found!");
			return false;
		}
		ads.setGain(GAIN_ONE); 
		return true;
	}

	int readADC(uint8_t channel) {
		int value = ads.readADC_SingleEnded(channel);
		if (value < 0 || value > 32767) return -1;
		return value;
	}

};
