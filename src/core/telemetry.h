#pragma once
#include <Arduino.h>
#include "posture_ids.h"
#include "matrix_data.h"
#include "fsr_data.h"
#include "tof_data.h"
// #include "imu_data.h"


struct Telemetry {
  uint16_t schema = 1;
  uint32_t t_ms   = 0;

  PosId posId = PosId::UNKNOWN;

  MatrixData matrix;
  FsrData    fsr;
  TofData    tof;
//   ImuData    imu;
};