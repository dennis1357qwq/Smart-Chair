#include <Arduino.h>
#include "matrix_posture.h"

MatrixPosture::MatrixPosture(const int* matrixPtr, uint8_t rows, uint8_t cols)
: _matrix(matrixPtr), _rows(rows), _cols(cols) {
  beginBaseline();
}

void MatrixPosture::beginBaseline() {
	const uint16_t N = _rows*_cols;
    for (uint16_t i=0;i<N;++i) {
		 _baseline[i]=0.f; _count[i]=0; 
	}
}

void MatrixPosture::feedBaselineSample() {
	if (!_matrix) return;
    const uint16_t N = _rows*_cols;
    for (uint16_t i=0;i<N;++i){
		int v = _matrix[i];
      	if (v == INVALID_INT) continue;
      	uint16_t& n = _count[i];
      	n++;
      	float b = _baseline[i];
      	_baseline[i] = b + (float(v) - b) / float(n);
    }
}

