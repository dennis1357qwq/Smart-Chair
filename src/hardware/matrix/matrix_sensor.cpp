#include "matrix_sensor.h"
#include "hardware/adc/adc_manager.h"
#include <Wire.h>
#include "hardware/mux.h"

Matrix_sensor::Matrix_sensor(Mux& rowMux, Mux& colMux, int adc_channel, Adc& adc, uint8_t rows, uint8_t cols)
: rowMux(rowMux), colMux(colMux), adsChannel(adc_channel), adc(adc), ROWS(rows), COLS(cols) {}

void Matrix_sensor::initMatrixSensor(){
    colMux.begin();
    rowMux.begin();
}

void Matrix_sensor::update_Matrix(){
    Serial.println("[");
    for (int col = 0; col < COLS; col++)
    {
        colMux.selectChannel(col);
        delay(2);

        for (int row = 0; row < ROWS; row++) {
            rowMux.selectChannel(row);
            delay(2);

            int value = adc.readADC(adsChannel);
            Serial.print(value);
            if (row < ROWS - 1) Serial.print(",");
        }

        if (col < COLS - 1) Serial.print(";");
    }
    Serial.println("]");
}

void Matrix_sensor::test_single_cell(uint8_t row, uint8_t col){
    rowMux.selectChannel(row);
    delay(2);

    colMux.selectChannel(col);
    delay(2);

    int value = adc.readADC(adsChannel);
    Serial.print("row: ");
    Serial.print(row);
    Serial.print("; col: ");
    Serial.print(col);
    Serial.print(": ");
        if (value < 0 || value > 32767) {
            Serial.println("X\t");  // mask = X
        } else {
            char buf[8];
            sprintf(buf, "%4d", value); 
            Serial.print(buf);
            Serial.println("\t");
        }
}

void Matrix_sensor::update_Matrix_HumanReadable(){
    Serial.println("=== Druckmatrix ===");
    Serial.print("Row\\Col\t");
    for (int col = 0; col < COLS; col++) {
        Serial.print(col);
        Serial.print("\t");
    }
    Serial.println();
    Serial.println();

    for (int row = 0; row < ROWS; row++) {
        Serial.print(row);
        Serial.print(":\t");

        for (int col = 0; col < COLS; col++) {
            colMux.selectChannel(col);
            delay(2);

            rowMux.selectChannel(row);
            delay(2);

            int value = adc.readADC(adsChannel);
            if (value < 0 || value > 32767) {
                Serial.print("X\t");  // mask = X
            } else {
                char buf[8];
                sprintf(buf, "%4d", value); 
                Serial.print(buf);
                Serial.print("\t");
            }
        }
        Serial.println();
    }

    Serial.println("===================");
}

