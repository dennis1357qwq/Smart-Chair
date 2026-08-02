#include "matrix_sensor.h"
#include "hardware/adc/adc_manager.h"
#include <Wire.h>
#include "hardware/mux.h"

MatrixSensor::MatrixSensor(Mux& rowMux, Mux& colMux, uint8_t adc_channel, Adc& adc, uint8_t rows, uint8_t cols)
: _rowMux(rowMux), _colMux(colMux), _adsChannel(adc_channel), _adc(adc), _ROWS(rows), _COLS(cols) {}

void MatrixSensor::initMatrixSensor(){
    _colMux.begin();
    _rowMux.begin();
}

void MatrixSensor::update(Telemetry& t){
    for (uint8_t col = 0; col < _COLS; col++)
    {
        _colMux.selectChannel(col);
        delay(2);
        for (int row = 0; row < _ROWS; row++) {
            _rowMux.selectChannel(row);
            delay(2);
            int value = _adc.readADC(_adsChannel);
            const int idx = row * _COLS + col;
            if(idx < (int)(MatrixData::ROWS * MatrixData::COLS)) {
                t.matrix.values[idx] = value;
            }
        }
    }
   
}

// void MatrixSensor::update(Telemetry& t){
//     Serial.println("[");
//     for (int col = 0; col < _COLS; col++)
//     {
//         _colMux.selectChannel(col);
//         delay(2);

//         for (int row = 0; row < _ROWS; row++) {
//             _rowMux.selectChannel(row);
//             delay(2);

//             int value = _adc.readADC(_adsChannel);
//             Serial.print(value);
//             if (row < _ROWS - 1) Serial.print(",");
//         }

//         if (col < _COLS - 1) Serial.print(";");
//     }
//     Serial.println("]");
// }

void MatrixSensor::test_single_cell(uint8_t row, uint8_t col){
    _rowMux.selectChannel(row);
    delay(2);

    _colMux.selectChannel(col);
    delay(2);

    int value = _adc.readADC(_adsChannel);
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

void MatrixSensor::update_Matrix_HumanReadable(){
    Serial.println("=== Druckmatrix ===");
    Serial.print("Row\\Col\t");
    for (int col = 0; col < _COLS; col++) {
        Serial.print(col);
        Serial.print("\t");
    }
    Serial.println();
    Serial.println();

    for (int row = 0; row < _ROWS; row++) {
        Serial.print(row);
        Serial.print(":\t");

        for (int col = 0; col < _COLS; col++) {
            _colMux.selectChannel(col);
            delay(2);

            _rowMux.selectChannel(row);
            delay(2);

            int value = _adc.readADC(_adsChannel);
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

