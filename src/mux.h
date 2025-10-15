#pragma once
#include <Arduino.h>

struct Mux {
    uint8_t s0, s1, s2, s3; // The Multiplexer receives its 4 control pins (s0,s1,s2,s3)

    Mux(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3)
        : s0(s0), s1(s1), s2(s2), s3(s3) {}

    void begin() { //Begin sets the given control pins to output mode on the esp32
        pinMode(s0, OUTPUT);
        pinMode(s1, OUTPUT);
        pinMode(s2, OUTPUT);
        pinMode(s3, OUTPUT);
    }

    void selectChannel(uint8_t channel) { //select a channel between 0-15
        digitalWrite(s0, (channel >> 0) & 1);
        digitalWrite(s1, (channel >> 1) & 1);
        digitalWrite(s2, (channel >> 2) & 1);
        digitalWrite(s3, (channel >> 3) & 1);
    }
};