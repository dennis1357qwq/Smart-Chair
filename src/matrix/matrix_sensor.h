#pragma once

#include <Arduino.h>

void initMatrixSensor(); // init MUX
void update_Matrix();  // updates and prints Matrix TODO: print will not be needed in future since ESP will deliver Data trough Endpoint not Serial Output