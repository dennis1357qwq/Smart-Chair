# Smart Chair

Research prototype for real-time, sensor-based sitting-posture monitoring and ergonomic feedback. The chair combines a custom pressure-sensitive seat matrix with time-of-flight (ToF) distance sensors in the backrest. An ESP32 reads and interprets the sensors, exposes the current state over HTTP, and a React client displays posture feedback.

The project was developed as part of the bachelor thesis *Design and Implementation of a Sensor-Enhanced Smart Chair for Posture Monitoring and Ergonomic Feedback* at Technische Universität Berlin.

> [!IMPORTANT]
> This is a research prototype, not a medical device. It has not been validated in a controlled multi-user study and must not be used for diagnosis or prescriptive health advice.

## System overview

```text
Pressure matrix ─┐
                 ├─> ESP32 ─> baseline-relative rules ─> posture state
Backrest ToF ────┘      │                               │
                        └──────── HTTP / Wi-Fi ─────────┘
                                          │
                                  React web client
```

The ESP32 operates independently of the client. It performs sensor acquisition, calibration, feature extraction, and deterministic posture classification locally. The client polls the chair and adds live display, chair-configuration guidance, and session-based feedback.

The currently active hardware configuration is:

- ESP32 development board using the Arduino framework
- custom 7×7 piezoresistive pressure matrix in the seat
- two CD74HC4067 analog multiplexers for matrix row/column selection
- ADS1115 16-bit analog-to-digital converter
- six VL53L1X ToF sensors in a 2×3 backrest arrangement
- one PCA9548A I²C multiplexer for the active backrest sensors
- three supplementary FSR inputs, currently exposed as telemetry but not used by the posture classifier

Additional head, knee, and bottom ToF groups exist in the data model and were considered during development. Their second I²C multiplexer and sensor registrations are disabled in the current firmware and they do not participate in classification.

More detail is available in:

- [Hardware and wiring](docs/hardware.md)
- [Software architecture](docs/architecture.md)
- [HTTP API](docs/api.md)
- [Known limitations](docs/limitations.md)

## Repository layout

```text
.
├── src/                    ESP32 firmware
│   ├── core/               telemetry, baseline and posture logic
│   ├── hardware/           sensor and multiplexer drivers
│   └── interface/          embedded HTTP server
├── smart-chair-frontend/   React/Vite client and experimental Electron shell
├── matrix-viewer.py       serial pressure-matrix visualization
├── multi-viewer.py        combined serial sensor visualization
├── tof-viewer.py          serial ToF visualization
├── platformio.ini         firmware build configuration
└── docs/                  technical handover documentation
```

## Firmware setup

### Prerequisites

- Python 3
- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) or the PlatformIO IDE extension
- a USB connection to the ESP32
- a 2.4 GHz Wi-Fi network accessible by both chair and client

Create the local Wi-Fi configuration:

```sh
cp src/secrets.h.example src/secrets.h
```

Then replace the placeholders in `src/secrets.h`:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
```

The file is ignored by Git. Do not commit real credentials.

Build and upload the firmware:

```sh
pio run
pio run --target upload
pio device monitor --baud 115200
```

On startup, the serial monitor prints the IP address assigned to the chair. The current firmware waits until Wi-Fi connects, so an incorrect or unavailable network prevents startup from completing.

## Client setup

### Prerequisites

- Node.js 20.19 or newer
- npm

```sh
cd smart-chair-frontend
npm ci
npm exec vite
```

Open the URL printed by Vite. To connect to the physical chair, create a local environment file:

```sh
cp .env.example .env.development.local
```

Then enter the IP address printed by the ESP32 serial monitor in the local file:

```env
VITE_CHAIR_BASE_URL=http://192.168.x.x
```

Restart Vite after changing the value. Local environment files are ignored by Git. If the variable is empty, Vite's development middleware supplies mock chair telemetry instead.

The Electron wrapper is experimental and is not currently considered a supported run target.

For a production web build:

```sh
npm run build
npm run preview
```

## Python visualization tools

The serial diagnostic viewers require Python 3 and the packages listed in `requirements.txt`. A virtual environment keeps them separate from system packages:

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -r requirements.txt
```

Run the combined viewer with:

```sh
python3 multi-viewer.py
```

Run the ToF-specific viewer with an explicit serial port:

```sh
python3 tof-viewer.py --port /dev/tty.usbserial-110 --baud 115200
```

Use `pio device list` or the PlatformIO device list in VS Code to find the current port. `matrix-viewer.py` and `multi-viewer.py` still define their serial port near the top of each script and must be adjusted there before use.

## Calibration and normal operation

1. Adjust the chair to the user before calibration.
2. Sit still in a comfortable, neutral posture with the back in its intended reference position.
3. Start calibration from the client or with `GET /baseline/start`.
4. Remain still for the default 15-second sampling period.
5. Wait until `/baseline/status` reports that calibration has finished.

The firmware averages matrix and backrest measurements during calibration. Later readings are interpreted as deviations from these user-specific references. Recalibrate when the user changes or after changing seat, backrest, or lumbar-support settings.

## Development checks

Firmware:

```sh
pio run
```

Native posture-logic tests (no ESP32 or PlatformIO required):

```sh
test/run-native-tests.sh
```

The native suite compiles the platform-independent C++ interpretation modules with the host compiler. It covers matrix zones and baselines, matrix and ToF tag extraction, back meta-features, and posture classification. It does not exercise physical sensors, I²C/ADC drivers, Wi-Fi, or the embedded HTTP server.

Frontend:

```sh
cd smart-chair-frontend
npm run build
npm run lint
```

At the time of handover preparation, the native posture-logic tests, frontend production build, and frontend lint check succeed. Frontend component tests and hardware-in-the-loop firmware tests have not yet been introduced. See [Known limitations](docs/limitations.md).

## Current status

The prototype demonstrates the full path from multimodal sensing to embedded, explainable posture classification and user-facing feedback. Occupancy and forward sitting were the most robust states during exploratory testing. Fine lateral distinctions, leaning back, and lower-back slouch classification require further stabilization and formal validation.

The bachelor thesis is the authoritative source for the research rationale, design process, and evaluation. This repository documentation describes the operational state of the checked-in implementation.
