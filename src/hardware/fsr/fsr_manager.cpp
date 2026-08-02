#include "fsr_manager.h"
#include "core/fsr_data.h"
#include <Wire.h>


FsrManager::FsrManager(Adc& adc, uint8_t lord, uint8_t left, uint8_t right)
: _adc(adc), _lord(lord), _left(left), _right(right){};

void FsrManager::update(Telemetry& t){
	int lordVal = _adc.readADC(_lord);
	int leftVal  = _adc.readADC(_left);
    int rightVal = _adc.readADC(_right);

	t.fsr.values[FSR_LORD] = lordVal;
	t.fsr.values[FSR_LEFT] = leftVal;
	t.fsr.values[FSR_RIGHT] = rightVal;
}

void FsrManager::printFsrByIndex(int sensor){
	if(sensor > 3 || sensor < 1) return;

	int val = _adc.readADC(sensor);
	Serial.print("Sensor nr.");
	Serial.print(sensor);
	Serial.print(": ");
	Serial.println(val);
}

