#include <Arduino.h>
#include <Wire.h>

void testI2CAdress() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // Falls du andere Pins nutzt, hier anpassen

  Serial.println("Scanning I2C bus...");
  byte count = 0;

  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(address, HEX);
      count++;
    }
    delay(5);
  }

  if (count == 0) {
    Serial.println("Keine I2C-Geräte gefunden.");
  } else {
    Serial.print("Gefundene Geräte: ");
    Serial.println(count);
  }
}
