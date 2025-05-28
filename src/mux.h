#pragma once
#include <Arduino.h>

struct Mux {
    uint8_t s0, s1, s2, s3;
    uint8_t sig;

    Mux(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t sig)
        : s0(s0), s1(s1), s2(s2), s3(s3), sig(sig) {}

    void begin() {
        pinMode(s0, OUTPUT);
        pinMode(s1, OUTPUT);
        pinMode(s2, OUTPUT);
        pinMode(s3, OUTPUT);
        pinMode(sig, OUTPUT);
    }

    void selectChannel(uint8_t channel) {
        digitalWrite(s0, (channel >> 0) & 1);
        digitalWrite(s1, (channel >> 1) & 1);
        digitalWrite(s2, (channel >> 2) & 1);
        digitalWrite(s3, (channel >> 3) & 1);
    }

    void enable() { digitalWrite(sig, HIGH); }
    void disable() { digitalWrite(sig, LOW); }
};