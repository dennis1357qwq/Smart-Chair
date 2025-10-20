#pragma once
#include <vector>
#include "tof_sensor.h"

class ToFManager {
private:
    std::vector<ToFSensor> sensors;
    std::vector<uint8_t> pins;
    uint8_t baseAddress;
public:
	ToFManager(const std::vector<uint8_t>& xshutPins, uint8_t baseAddres = 0x30);
	void initToFSensors();
	void updateToFSensors();
};

