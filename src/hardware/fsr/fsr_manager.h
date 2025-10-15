#pragma once
#include <Arduino.h>
#include "hardware/adc/adc_manager.h"
#include "core/telemetry.h"

class FsrManager {
public: 
	FsrManager(Adc& adc, uint8_t lord, uint8_t left, uint8_t right);

	void update(Telemetry& t);
	void printFsrByIndex(int sensor);
	void printAllFsrs();
	void humanReadableSensors();

private:
	Adc& _adc;
	uint8_t _lord;
	uint8_t _left;
	uint8_t _right;
};