#pragma once
#include <Arduino.h>
#include "adc/adc_manager.h"


class Fsr_manager {
public: 
	Fsr_manager(Adc& adc, uint8_t lord, uint8_t left, uint8_t right);

	void printFsrByIndex(int sensor);
	void printAllFsrs();
	void humanReadableSensors();

private:
	Adc& adc;
	uint8_t lord;
	uint8_t left;
	uint8_t right;
};