#include <VL53L0X.h>

// put function declarations here:
VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

#define XSHUT1 16
#define XSHUT2 17
#define XSHUT3 18


// void setup() {
//   Serial.begin(115200);
//   Wire.begin();

//   // Setze die XSHUT-Pins als Ausgänge
//   pinMode(XSHUT1, OUTPUT);
//   pinMode(XSHUT2, OUTPUT);
//   // pinMode(XSHUT3, OUTPUT);

//   // Alle Sensoren ausschalten
//   digitalWrite(XSHUT1, LOW);
//   digitalWrite(XSHUT2, LOW);
//   // digitalWrite(XSHUT3, LOW);
//   delay(10); // Warte kurz

//   // Sensor 1 aktivieren und Adresse setzen
//   digitalWrite(XSHUT1, HIGH);
//   delay(10);
//   if (!sensor1.init()) {
//     Serial.println("Sensor 1 konnte nicht initialisiert werden.");
//     while (1);
//   }
//   sensor1.setAddress(0x30);  // Sensor 1 erhält Adresse 0x30

//   // Sensor 2 aktivieren und Adresse setzen
//   digitalWrite(XSHUT2, HIGH);
//   delay(10);
//   if (!sensor2.init()) {
//     Serial.println("Sensor 2 konnte nicht initialisiert werden.");
//     while (1);
//   }
//   sensor2.setAddress(0x31);  // Sensor 2 erhält Adresse 0x31

//   // Sensor 3 aktivieren und Adresse setzen
//   // digitalWrite(XSHUT3, HIGH);
//   // delay(10);
//   // if (!sensor3.init()) {
//   //   Serial.println("Sensor 3 konnte nicht initialisiert werden.");
//   //   while (1);
//   // }
//   // sensor3.setAddress(0x32);  // Sensor 3 erhält Adresse 0x32

//   Serial.println("Alle Sensoren initialisiert.");

//   sensor1.setMeasurementTimingBudget(200000);
//   sensor2.setMeasurementTimingBudget(200000);
//   // sensor3.setMeasurementTimingBudget(200000);
// }




// void loop() {
//   // Sensor 1 messen
//   uint16_t distance1 = sensor1.readRangeSingleMillimeters();
//   if (sensor1.timeoutOccurred()) {
//     Serial.println("Timeout bei Sensor 1");
//   } else {
//     Serial.print("Sensor 1: ");
//     Serial.print(distance1);
//     Serial.println(" mm");
//   }

//   // Sensor 2 messen
//   uint16_t distance2 = sensor2.readRangeSingleMillimeters();
//   if (sensor2.timeoutOccurred()) {
//     Serial.println("Timeout bei Sensor 2");
//   } else {
//     Serial.print("Sensor 2: ");
//     Serial.print(distance2);
//     Serial.println(" mm");
//   }

//   // Sensor 3 messen
//   // uint16_t distance3 = sensor3.readRangeSingleMillimeters();
//   // if (sensor3.timeoutOccurred()) {
//   //   Serial.println("Timeout bei Sensor 3");
//   // } else {
//   //   Serial.print("Sensor 3: ");
//   //   Serial.print(distance3);
//   //   Serial.println(" mm");
//   // }

//   delay(1000);  // Eine Sekunde zwischen den Messungen warten
// }