#include "tof_sensor.h"
#include <Arduino.h>

ToFSensor::ToFSensor(uint8_t xshut, uint8_t address) 
    : xshutPin(xshut), i2cAddress(address) {}

bool ToFSensor::init() {
    pinMode(xshutPin, OUTPUT);
    digitalWrite(xshutPin, LOW);
    delay(10);
    digitalWrite(xshutPin, HIGH);
    delay(10);

    if (!sensor.init()) {
        Serial.printf("Sensor an Pin %d konnte nicht initialisiert werden.\n", xshutPin);
        return false;
    }

    sensor.setAddress(i2cAddress);
    sensor.setMeasurementTimingBudget(200000);
    Serial.printf("✅ Sensor an Pin %d initialisiert (Adresse 0x%X)\n", xshutPin, i2cAddress);
    return true;
}

uint16_t ToFSensor::read() {
    uint16_t dist = sensor.readRangeSingleMillimeters();
    if (sensor.timeoutOccurred()) return 0xFFFF;
    return dist;
}