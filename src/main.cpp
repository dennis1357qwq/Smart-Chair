#include <Arduino.h>
#include <Wire.h>
#include "adc_manager.h"
#include "matrix_sensor.h"
#include "tof_manager.h"
#include <vector>



void setup() {
    Serial.begin(115200);
    delay(2500);
    Wire.begin(21, 22);
    delay(100);
    initToFSensors();
}

void loop() {
    Serial.println("- - - - - - - - - - - - - -");
    updateToFSensors();
    delay(100);
}