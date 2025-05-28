#include <Arduino.h>
#include <Wire.h>
#include "adc/adc_manager.h"
#include "matrix/matrix_sensor.h"
#include "tof/tof_manager.h"
#include <vector>

ToFManager tof({4, 15, 23, 13, 32, 33});

void setup() {
    Serial.begin(115200);
    delay(2500);
    Wire.begin(21, 22);
    delay(100);
    tof.initToFSensors();
}

void loop() {
    Serial.println("- - - - - - - - - - - - - -");
    tof.updateToFSensors();
    delay(100);
}