#include <Arduino.h>
#include <Wire.h>
#include "hardware/adc/adc_manager.h"
#include "hardware/matrix/matrix_sensor.h"
#include <vector>
#include "hardware/mux.h"
#include "core/telemetry.h"
#include "core/telemetry_json.h"
#include "hardware/fsr/fsr_manager.h"
#include "core/fsr_data.h"
#include "hardware/tof/i2c_mux_PCA9548A.h"
#include "hardware/tof/tof_manager.h"

Telemetry state;
Adc adc;
Mux colMux(25,33,32,18); // Multiplexer for measurement. Control pins(s0,s1,s2,s3).
Mux rowMux(26,27,14,12); // Multiplexer for Voltage delivery. 
PCA9548A i2cMux_1(0x70);
PCA9548A i2cMux_2(0x71);
ToFManager tofm;
MatrixSensor matrix(rowMux, colMux, 0, adc, 7, 7);
FsrManager fsr(adc, 1, 2, 3);

void setup() {
  Serial.begin(115200);
  delay(300);

  Wire.begin(21, 22);
  Wire.setClock(50000);
  delay(50);

  i2cMux_1.disableAll();
  delay(5);
  i2cMux_2.disableAll();
  delay(5);
  adc.initADC();
  matrix.initMatrixSensor();

  tofm.registerMux(i2cMux_1);
  tofm.registerMux(i2cMux_2);
  tofm.add(ToF(&i2cMux_1, 1, ToFType::L1X, ToFSlot::KNEE, 0));
  tofm.add(ToF(&i2cMux_1, 0, ToFType::L0X, ToFSlot::BACK, 0));
  tofm.add(ToF(&i2cMux_2, 2, ToFType::L0X, ToFSlot::HEAD, 0));

  tofm.init();
}

void loop() {   
    matrix.update(state);
    fsr.update(state);
    tofm.update(state);
    state.t_ms = millis();
    print_json(state, Serial);
    Serial.println(); 
    delay(500);
}