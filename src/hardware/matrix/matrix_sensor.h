#pragma once

#include <Arduino.h>
#include "hardware/mux.h"
#include "hardware/adc/adc_manager.h"
#include "core/telemetry.h"

class MatrixSensor {
public:
	MatrixSensor(Mux& rowMux, Mux& colMux, uint8_t adsChannel, Adc& adc, uint8_t rows, uint8_t cols);

	void initMatrixSensor();
	void update(Telemetry& t);
	void test_single_cell(uint8_t row, uint8_t col);
	void update_Matrix_HumanReadable();

private:
	Mux& _rowMux;
	Mux& _colMux;
	Adc& _adc;
	uint8_t _adsChannel;
	uint8_t _ROWS;
	uint8_t _COLS;
};