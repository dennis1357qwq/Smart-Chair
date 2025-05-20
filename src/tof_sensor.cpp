#include "tof_sensor.h"
#include <Arduino.h>

ToFSensor::ToFSensor(uint8_t xshut, uint8_t address) 
    : xshutPin(xshut), i2cAddress(address) {}

void ToFSensor::powerDown(){
    pinMode(xshutPin, OUTPUT);
    digitalWrite(xshutPin, LOW);
    delay(10);
}

bool ToFSensor::init() {
    digitalWrite(xshutPin, HIGH);
    delay(10);

    if (!sensor.init()) {
        Serial.printf("Sensor an Pin %d konnte nicht initialisiert werden.\n", xshutPin);
        return false;
    }

    sensor.setAddress(i2cAddress);
    sensor.setMeasurementTimingBudget(200000);
    Serial.printf("✅ Sensor an Pin %d initialisiert (Adresse 0x%X)\n", xshutPin, i2cAddress);
    initialized = true;
    return true;
}

uint16_t ToFSensor::read() {
    if(!initialized) return 0xFFFF;
    uint16_t dist = sensor.readRangeSingleMillimeters();
    return sensor.timeoutOccurred() ? 0xFFFF : dist;
}