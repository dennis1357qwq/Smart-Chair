#include "tof_manager.h"
#include <Arduino.h>

static std::vector<ToFSensor> tofSensors;
static const uint8_t NUM_OF_TOF_SENSORS = 5;
static const uint8_t xshutPins[NUM_OF_TOF_SENSORS] = {4, 15, 23};
static const uint8_t i2cAddresses[NUM_OF_TOF_SENSORS] = {0x30, 0x31, 0x32};

void initToFSensors() {
    tofSensors.clear();

    for (uint8_t i = 0; i < 1; ++i) {
        tofSensors.emplace_back(xshutPins[i], i2cAddresses[i]);

        if (!tofSensors[i].init()) {
            Serial.printf("Init fehlgeschlagen für Sensor %zu (Pin %d)\n", i, xshutPins[i]);
              // Optional: anhalten bei Init-Fehler
        }
    }

    Serial.println("Alle ToF-Sensoren erfolgreich initialisiert.");
}

void updateToFSensors() {
    for (size_t i = 0; i < 1; ++i) {
        uint16_t dist = tofSensors[i].read();
        if (dist == 0xFFFF) {
            Serial.printf("Sensor %d: Timeout oder Fehler\n", i);
        } else {
            Serial.printf("Sensor %d: %d mm\n", i, dist);
        }
    }
}