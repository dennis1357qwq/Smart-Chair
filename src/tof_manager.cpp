#include "tof_manager.h"
#include <Arduino.h>

static std::vector<ToFSensor> tofSensors;
static const uint8_t NUM_OF_TOF_SENSORS = 6;
static const uint8_t xshutPins[NUM_OF_TOF_SENSORS] = {4, 15, 23, 13, 32, 33};
static const uint8_t BASE_TOF_ADRESS = 0x30;

void initToFSensors() {
    tofSensors.clear();

    for (size_t i = 0; i < NUM_OF_TOF_SENSORS; i++)
    {
        tofSensors.emplace_back(xshutPins[i], BASE_TOF_ADRESS+i);
        tofSensors[i].powerDown();
    }
    Serial.println("Alle Sensoren down");
    delay(10);

    for (uint8_t i =0; i < NUM_OF_TOF_SENSORS; ++i) {
        if (!tofSensors[i].init()) {
            Serial.printf("Init fehlgeschlagen für Sensor %zu (Pin %d, Adresse %d)\n", i, tofSensors[i].xshutPin, tofSensors[i].i2cAddress);
              // Optional: anhalten bei Init-Fehler
        }
    }

    Serial.println("Initialisierung abgeschlossen.");
}

void updateToFSensors() {
    for (size_t i = 0; i < NUM_OF_TOF_SENSORS; ++i) {
        uint16_t dist = tofSensors[i].read();
        if (dist == 0xFFFF) {
            Serial.printf("Sensor %d: Timeout oder Fehler\n", i);
        } else {
            Serial.printf("Sensor %d: %d mm\n", i, dist);
        }
    }
}