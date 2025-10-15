#pragma once

#include <Arduino.h>
#include "mux.h"
#include "adc/adc_manager.h"

class Matrix_sensor {
public:
	Matrix_sensor(Mux& rowMux, Mux& colMux, int adsChannel, Adc& adc, uint8_t rows, uint8_t cols);

	void initMatrixSensor();
	void update_Matrix(); // updates and prints Matrix TODO: print will not be needed in future since ESP will deliver Data trough Endpoint not Serial Output
	void test_single_cell(uint8_t row, uint8_t col);
	void update_Matrix_HumanReadable();

private:
	Mux& rowMux;
	Mux& colMux;
	int adsChannel;
	Adc& adc;
	uint8_t ROWS;
	uint8_t COLS;
};