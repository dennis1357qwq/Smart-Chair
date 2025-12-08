#pragma once
#include <Arduino.h>
#include "core/posture/matrix_posture.h"
#include "core/posture/tof_posture.h"   // Pfad ggf. anpassen

class BaselineManager {
public:
  enum class State : uint8_t { IDLE, RUNNING };

  State state() const { return _state; }

  // ToF ist optional: du kannst den zweiten Parameter auch später via attachToF setzen.
  BaselineManager(MatrixPosture& matrix, ToFPosture* tof = nullptr)
  : _matrix(matrix), _tof(tof) {}

  // Falls du ToF erst nachträglich anhängen willst
  void attachToF(ToFPosture& tof) { _tof = &tof; }

  // Startet eine neue Baseline-Phase
  void start(uint32_t durationMs,
             bool forMatrix,
             bool forTof);

  void update();

  bool isRunning() const { return _state == State::RUNNING; }

  uint32_t remainingMs() const {
    if (_state != State::RUNNING) return 0;
    uint32_t passed = millis() - _startMs;
    return (passed >= _durationMs) ? 0 : (_durationMs - passed);
  }

  bool hasMatrixBaseline() const { return _hasMatrixBaseline; }
  bool hasTofBaseline()    const { return _hasTofBaseline; }

  // „Rising edge“: genau einmal true direkt nach Abschluss
  bool baselineJustFinished();

  MatrixPosture& matrixPosture() { return _matrix; }
  ToFPosture*    tofPosture()    { return _tof; }

private:
  State _state = State::IDLE;
  uint32_t _startMs   = 0;
  uint32_t _durationMs = 0;

  bool _useMatrix = false;
  bool _useToF    = false;

  bool _hasMatrixBaseline = false;
  bool _hasTofBaseline    = false;

  MatrixPosture& _matrix;
  ToFPosture*    _tof = nullptr;

  bool _justFinished = false;
};