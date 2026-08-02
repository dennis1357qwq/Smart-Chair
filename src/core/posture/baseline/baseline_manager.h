#pragma once
#include "core/posture/matrix/matrix_posture.h"
#include "core/posture/tof/tof_posture.h"
#include <Arduino.h>

class BaselineManager {
public:
  enum class State : uint8_t { IDLE, RUNNING };

  State state() const { return _state; }

  BaselineManager(MatrixPosture &matrix, ToFPosture *tof = nullptr)
      : _matrix(matrix), _tof(tof) {}

  void attachToF(ToFPosture &tof) { _tof = &tof; }
  void start(uint32_t durationMs, bool forMatrix, bool forTof);

  void update();

  bool isRunning() const { return _state == State::RUNNING; }

  uint32_t remainingMs() const {
    if (_state != State::RUNNING)
      return 0;
    uint32_t passed = millis() - _startMs;
    return (passed >= _durationMs) ? 0 : (_durationMs - passed);
  }

  bool hasMatrixBaseline() const { return _hasMatrixBaseline; }
  bool hasTofBaseline() const { return _hasTofBaseline; }
  bool baselineJustFinished();

  MatrixPosture &matrixPosture() { return _matrix; }
  ToFPosture *tofPosture() { return _tof; }

private:
  State _state = State::IDLE;
  uint32_t _startMs = 0;
  uint32_t _durationMs = 0;

  bool _useMatrix = false;
  bool _useToF = false;

  bool _hasMatrixBaseline = false;
  bool _hasTofBaseline = false;

  MatrixPosture &_matrix;
  ToFPosture *_tof = nullptr;

  bool _justFinished = false;
};