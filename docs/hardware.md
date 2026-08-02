# Hardware and wiring

This document describes the hardware configuration represented by the current firmware. It distinguishes active components from experimental components described in the thesis but disabled in `src/main.cpp`.

## Active components

| Component | Purpose | Connection/configuration |
| --- | --- | --- |
| ESP32 development board | acquisition, interpretation, Wi-Fi API | Arduino/PlatformIO target `esp32dev` |
| Custom 7×7 pressure matrix | seat-pressure distribution | 7 row and 7 column electrodes |
| 2× CD74HC4067 | select matrix row and column | four GPIO address lines per multiplexer |
| ADS1115 | digitize matrix and FSR signals | I²C; matrix uses single-ended channel 0 |
| 6× VL53L1X | backrest distances | PCA9548A channels 0, 1, 2, 5, 6, 7 |
| PCA9548A at `0x70` | isolate equal-address ToF sensors | I²C |
| 3× FSR inputs | supplementary contact measurements | ADS1115 channels 1, 2, 3 |

## ESP32 pin assignment

| Function | Pins |
| --- | --- |
| I²C SDA | GPIO 21 |
| I²C SCL | GPIO 22 |
| column multiplexer S0–S3 | GPIO 25, 33, 32, 18 |
| row multiplexer S0–S3 | GPIO 12, 14, 27, 26 |

The I²C clock is configured to 100 kHz. Pin assignments are currently constructed directly in `src/main.cpp`; this table must be updated if that configuration changes.

## Pressure matrix

The final seat sensor is a custom 7×7 piezoresistive matrix covering approximately 46 cm × 42 cm. Each cell is formed by conductive copper electrodes with Velostat as the pressure-sensitive material.

The original 10×11 continuous-layer prototype suffered from electrical ghosting and mechanical crosstalk under seated load. The revised matrix uses:

- seven independent row modules with seven 3 cm × 3 cm sensing pads each;
- 1N5817 Schottky diodes between each row pad and its row bus to prevent reverse current paths;
- mechanically separated, peninsula-shaped pad carriers;
- shaped and serpentine column conductors to reduce mechanical coupling between neighboring cells.

Firmware activates one row at a time, selects each column, reads the ADS1115, and stores the result as a row-major 7×7 array.

## Backrest ToF layout

Six VL53L1X sensors are arranged as two vertical columns of three sensors. Together they represent upper, middle, and lower back zones. The firmware maps them to `TofData::back[0..5]` in registration order:

| Telemetry index | PCA9548A channel |
| --- | --- |
| 0 | 0 |
| 1 | 1 |
| 2 | 2 |
| 3 | 5 |
| 4 | 6 |
| 5 | 7 |

Before changing this mapping, confirm the physical left/right and upper/middle/lower placement on the prototype. That physical orientation is not encoded unambiguously in the repository.

## Inactive and experimental components

The software data model also provides head, knee, and bottom arrays. The thesis describes a second PCA9548A at `0x71`, two forward-facing VL53L0X knee sensors, and two downward-facing VL53L1X sensors. In the current checkout:

- the second multiplexer declaration is commented out;
- all knee and bottom sensor registrations are commented out;
- these measurements do not participate in posture classification;
- FSR values are acquired but are not used by the classifier.

Treat these as extension points, not as active features.

## Power and physical maintenance

The prototype ESP32 is powered through Micro-USB, either from a development computer or a power bank. Long ribbon cables and flexing sensor layers are mechanically sensitive. Before diagnosing software, inspect:

- USB power and common ground;
- I²C and multiplexer connections;
- ribbon-cable strain and solder joints;
- copper-tape continuity;
- diode joints in the matrix;
- unobstructed ToF sensor fields of view.

The backrest is mesh-based. Sensor position and unobstructed optical paths matter and should be preserved during mechanical changes.
