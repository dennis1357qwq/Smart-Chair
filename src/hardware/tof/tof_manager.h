#pragma once
#include "core/telemetry.h"
#include "i2c_mux_PCA9548A.h"
#include <Arduino.h>
#include <VL53L0X.h>
#include <VL53L1X.h>

enum class ToFType : uint8_t { L0X = 0, L1X = 1 };
enum class ToFSlot : uint8_t { BACK = 0, HEAD = 1, KNEE = 2, BOTTOM = 3 };

struct ToF {
  PCA9548A *mux = nullptr;      // mux addr
  uint8_t muxCh = 0;            // mux channel
  ToFType type = ToFType::L0X;  // L0X or L1X
  ToFSlot slot = ToFSlot::BACK; // telemetry
  uint8_t idx = 0;              // index in type array
  bool valid = false;           // init() worked?

  VL53L0X *l0 = nullptr;
  VL53L1X *l1 = nullptr;

  ToF() = default;

  ToF(PCA9548A *m, uint8_t ch, ToFType ty, ToFSlot sl, uint8_t i)
      : mux(m), muxCh(ch), type(ty), slot(sl), idx(i) {}
};

class ToFManager {
public:
  static constexpr uint8_t MAX_TOF = 16;

  static constexpr uint8_t MAX_L0X = MAX_TOF;
  static constexpr uint8_t MAX_L1X =
      MAX_TOF; // Pools with Sensor instances - could be reduced to actuall
               // amounts of sensors if clear
  ToFManager();

  bool add(const ToF &tof); // add a tof to the list of all ToFs
  void init(); // logically seperates between two types of sensors and uses
               // specific init for each sensor
  void update(Telemetry &t);
  void registerMux(PCA9548A &m);

private:
  ToF _tofs[MAX_TOF];
  uint8_t _count = 0;
  VL53L0X _l0xPool[MAX_L0X];
  uint8_t _l0xN = 0;
  VL53L1X _l1xPool[MAX_L1X];
  uint8_t _l1xN = 0;
  PCA9548A *_allMux[4] = {nullptr}; // up to 4 mux
  uint8_t _allMuxN = 0;

  void writeToTelemetry(const ToF &tof, Telemetry &t, int value);
  void selectOnly(PCA9548A *m, uint8_t ch);
};