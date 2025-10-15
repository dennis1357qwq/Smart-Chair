#include "fsr_manager.h"
#include <Wire.h>

Fsr_manager::Fsr_manager(Adc& adc, uint8_t lord, uint8_t left, uint8_t right)
: adc(adc), lord(lord), left(left), right(right){};

void Fsr_manager::printFsrByIndex(int sensor){
	if(sensor > 3 || sensor < 1) return;

	int val = adc.readADC(sensor);
	Serial.print("Sensor nr.");
	Serial.print(sensor);
	Serial.print(": ");
	Serial.println(val);
}

