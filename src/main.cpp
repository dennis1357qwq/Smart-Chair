#include <Arduino.h>
#include <Wire.h>
#include "adc/adc_manager.h"
#include "matrix/matrix_sensor.h"
#include "tof/tof_manager.h"
#include <vector>

ToFManager tof({33, 32, 13, 23, 15, 4});

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