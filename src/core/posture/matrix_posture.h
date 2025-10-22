#pragma once
#include <Arduino.h>
#include <vector>

struct MatrixTags {
	// basic directional tags
  	bool weightLeft=false, weightRight=false, weightForward=false, weightBackward=false;
	bool neutral=false;

	// Advanced tags
	bool crossedLegLeft=false, crossedLegRight=false;
	bool sitOnLegLeft=false,  sitOnLegRight=false;

	// Debug/Kennzahlen
	float lrIndex=0.f, fbIndex=0.f, deltaSum=0.f, cx=0.5f, cy=0.5f;
};

class MatrixPosture {
public:
	static constexpr int INVALID_INT = -1;
	MatrixPosture(const int* matrixPtr, uint8_t rows, uint8_t cols);

	// Compute Baseline
	void beginBaseline();
	void feedBaselineSample();
	void finalizeBaseline();

	const float* getBaseline() const {return _baseline;}
	const uint16_t* getCounts() const { return _count; }
	
private:
	const int* _matrix;
	uint16_t _rows, _cols;
	uint16_t _frames = 0;
	
	float _baseline[49];
	uint16_t _count[49];
};