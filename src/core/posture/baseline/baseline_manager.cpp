#include "baseline_manager.h"

void BaselineManager::start(uint32_t durationMs, bool forMatrix, bool forTof) {
  if (_state == State::RUNNING)
    return;

  _state = State::RUNNING;
  _startMs = millis();
  _durationMs = durationMs;
  _justFinished = false;

  _useMatrix = forMatrix;
  _useToF = forTof && (_tof != nullptr);

  if (_useMatrix) {
    _matrix.beginBaseline();
    _hasMatrixBaseline = false;
  }
  if (_useToF && _tof) {
    _tof->beginBaseline();
    _hasTofBaseline = false;
  }
}

void BaselineManager::update() {
  if (_state != State::RUNNING)
    return;

  if (_useMatrix) {
    _matrix.feedBaselineSample();
  }
  if (_useToF && _tof) {
    _tof->feedBaselineSample();
  }

  if (millis() - _startMs >= _durationMs) {
    if (_useMatrix) {
      _matrix.finalizeBaseline();
      _hasMatrixBaseline = true;
    }
    if (_useToF && _tof) {
      _tof->finalizeBaseline();
      _hasTofBaseline = true;
    }

    _state = State::IDLE;
    _justFinished = true;
  }
}

bool BaselineManager::baselineJustFinished() {
  if (_justFinished) {
    _justFinished = false;
    return true;
  }
  return false;
}