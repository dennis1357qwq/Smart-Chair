# Tests

## Native posture-logic suite

Run from the repository root:

```sh
test/run-native-tests.sh
```

Requirements:

- a C++17 compiler available as `c++`, or set via `CXX`;
- no ESP32, PlatformIO installation, network connection, or sensor hardware.

The script builds into a temporary file and removes the binary afterward. Compiler warnings are treated as errors.

Covered modules:

- 7×7 matrix-zone construction;
- matrix and ToF baseline averaging;
- invalid measurement handling;
- occupancy, neutral, and directional matrix tags;
- ToF presence, contact, neutral, and torso-direction tags;
- back slouch, hyperlordosis, and twist meta-features;
- current rule-based posture classifier branches.

Not covered:

- ESP32 startup and timing behavior;
- GPIO, multiplexers, ADS1115, and physical sensors;
- Wi-Fi and the embedded HTTP server;
- telemetry serialization through Arduino `Stream`;
- frontend components and browser behavior.
