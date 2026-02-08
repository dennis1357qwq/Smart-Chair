#include "tof_posture.h"

ToFPosture::ToFPosture(const TofData *tofPtr) : _tof(tofPtr) {
  beginBaseline();
}