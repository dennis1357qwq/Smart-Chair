#include <Arduino.h>
#include <Wire.h>
#include "adc_manager.h"
#include "matrix_sensor.h"
#include "tof_sensor.h"
#include "tof_manager.h"
#include <vector>

VL53L0X sensor;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    delay(100);
    initToFSensors();
}

void loop() {
    updateToFSensors();
}