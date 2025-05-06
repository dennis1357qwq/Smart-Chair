#include "matrix_sensor.h"
#include "adc_manager.h"
#include <Wire.h>

const int rowMuxPins[4] = {26,27,14,12}; //  control Row-MUX
const int colMuxPins[4] = {19, 18, 5, 17}; // control Col-MUX
const int colMuxSIG = 0;  // A0 for ADS1115
const int ROWS = 10;
const int COLS = 11;

void selectMuxChannel(int channel, const int* muxPins) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(muxPins[i], (channel >> i) & 1);
    }
}

void init_Matrix() {
    //MUX control pins set to outputs
    for (int i = 0; i < 4; i++) {
        pinMode(rowMuxPins[i], OUTPUT);
        pinMode(colMuxPins[i], OUTPUT);
    }
}

void update_Matrix() {
    for (int col = 0; col < COLS; col++) {
        selectMuxChannel(col, colMuxPins); // pick col to measure
        delayMicroseconds(300);

        for (int row = 0; row < ROWS; row++) {
            selectMuxChannel(row, rowMuxPins); // pick row for 3.3V delivery
            delayMicroseconds(300);

            int value = readADC(colMuxSIG);
            if (value < 0 || value > 32767) value = -1;
            Serial.print(value);
            if (row < ROWS - 1) Serial.print(",");
        }

        if (col < COLS - 1) Serial.print(";");
    }
    Serial.println("]");
}