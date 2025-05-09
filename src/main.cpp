#include <Arduino.h>
#include <Wire.h>
#include "adc_manager.h"
#include "matrix_sensor.h"

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    initADC();
    initMatrixSensor();
}

void loop() {
    update_Matrix();
    delay(50);
}


