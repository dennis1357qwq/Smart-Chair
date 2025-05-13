#pragma once
#include <VL53L0X.h>

struct ToFSensor {
    VL53L0X sensor;
    uint8_t xshutPin;
    uint8_t i2cAddress;

    ToFSensor(uint8_t xshutPin, uint8_t i2cAddress);

    bool init();
    uint16_t read();
};
