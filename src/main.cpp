#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "matrix_sensor.h"

// const int SIGNAL_PIN = 34;

// Multiplexer Steuerpins
const int rowMuxPins[4] = {26,27,14,12}; // Steuert Zeilen-MUX
const int colMuxPins[4] = {19, 18, 5, 17}; // Steuert Spalten-MUX

// Multiplexer SIG-Pin
const int colMuxSIG = 0;  // A0 am ADS1115

// Matrixgröße
const int ROWS = 10;
const int COLS = 11;

// ADC-Objekt
Adafruit_ADS1115 ads;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    initMatrixSensor();
    // ADC starten
    // if (!ads.begin()) {
    //     Serial.println("Fehler: ADS1115 nicht gefunden!");
    //     while (1);
    // }
    // ads.setGain(GAIN_ONE); // Verstärkung 1 für 0-4.096V

    // // Multiplexer Steuerpins als Ausgang setzen
    // for (int i = 0; i < 4; i++) {
    //     pinMode(rowMuxPins[i], OUTPUT);
    //     pinMode(colMuxPins[i], OUTPUT);
    // }
}

// Funktion zum Setzen des Multiplexer-Kanals
void selectMuxChannel(int channel, const int* muxPins) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(muxPins[i], (channel >> i) & 1);
    }
}

void loop() {
    // Serial.println("[");
    // int matrix[ROWS][COLS];

    // // Spaltenbasiertes Scannen
    // for (int col = 0; col < COLS; col++) {
    //     selectMuxChannel(col, colMuxPins); // Wähle Spalte (zum Messen)
    //     delayMicroseconds(300);

    //     for (int row = 0; row < ROWS; row++) {
    //         selectMuxChannel(row, rowMuxPins); // Wähle Zeile (wird mit Spannung versorgt)
    //         delayMicroseconds(300);

    //         int value = ads.readADC_SingleEnded(colMuxSIG);
    //         if (value < 0 || value > 32767) value = -1;
    //         Serial.print(value);
    //         if (row < ROWS - 1) Serial.print(",");
    //     }

    //     if (col < COLS - 1) Serial.print(";");
    // }
    // Serial.println("]");
    update_Matrix();
    delay(50);
}


