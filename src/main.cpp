#include <Arduino.h>
#include <Wire.h>
#include "hardware/adc/adc_manager.h"
#include "hardware/matrix/matrix_sensor.h"
#include "hardware/tof/tof_manager.h"
#include <vector>
#include "hardware/mux.h"
#include "core/telemetry.h"
#include "core/telemetry_json.h"
#include "hardware/fsr/fsr_manager.h"
#include "core/fsr_data.h"

Telemetry state;
Adc adc;
Mux colMux(25,33,32,18); // Multiplexer for measurement. Control pins(s0,s1,s2,s3).
Mux rowMux(26,27,14,12); // Multiplexer for Voltage delivery. 
MatrixSensor matrix(rowMux, colMux, 0, adc, 7, 7);
FsrManager fsr(adc, 1, 2, 3);


// ToFManager tof({33, 32, 13, 23, 15, 4});
// ToFManager tof({32, 33, 13, 14, 27, 4});

void setup() {
    Serial.begin(115200);
    delay(2500);
    Wire.begin(21, 22);
    delay(100);
    adc.initADC(); // hier muss noch geklärt werden wer abhängig von dieser init ist und deshalb mit in den begin muss
    matrix.initMatrixSensor();
    // tof.initToFSensors();
}

void loop() {   
    // tof.updateToFSensors();
    matrix.update(state);
    fsr.update(state);
    state.t_ms = millis();
    print_json(state, Serial);
    Serial.println(); 
    // update_Matrix_HumanReadable();
    delay(500);
}