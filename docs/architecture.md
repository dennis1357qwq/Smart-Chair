# Software architecture

## Data flow

```text
Sensor drivers
  ├─ pressure matrix
  ├─ ToF manager
  └─ FSR manager
          │
          v
      Telemetry state
          │
          ├─> baseline manager
          ├─> matrix and ToF feature extraction
          ├─> rule-based posture classifier
          └─> JSON serializer -> embedded HTTP server -> client
```

## Firmware layers

### Sensor and acquisition layer

`src/hardware` contains low-level interaction with the matrix, ADC, FSRs, ToF sensors, and multiplexers. Each loop iteration updates the shared `Telemetry` instance.

### Processing and interpretation layer

`src/core/posture` converts raw readings into interpretable features:

1. `BaselineManager` samples and finalizes user-specific matrix and ToF references.
2. `MatrixPosture` computes baseline-relative cell deltas, aggregates spatial zones, and emits matrix tags such as occupancy and directional weight shift.
3. `ToFPosture` evaluates backrest distances relative to baseline and emits contact/distance tags.
4. Backrest meta-feature logic combines individual ToF tags into higher-level back states.
5. `PostureClassifier` fuses matrix and backrest features into one `PosId`.

The approach is deterministic and explainable: each posture state results from explicit thresholds and rules. Thresholds were tuned empirically and are not statistically validated.

### Interface layer

The ESP32 hosts a synchronous HTTP server on port 80. `Telemetry` is both the internal shared state and the source for API serialization, so clients receive the same readings and posture result used by the embedded classifier.

## Client application

The React/Vite application polls `/telemetry` and provides:

- connection and calibration controls;
- current posture display;
- user-defined sessions and timers;
- configurable posture warnings and violations;
- a static ergonomic chair-configuration guide.

Session data is currently kept in browser storage. There is no backend database, account system, or cloud dependency.

Vite provides a development-only mock API when no chair base URL is configured. This allows most client work without physical hardware.

The Electron directory is an experimental wrapper and is not currently a supported packaging path.

## Telemetry boundary

The JSON object contains:

- schema version and ESP32 uptime timestamp;
- numeric and textual posture identifiers;
- row-major pressure-matrix dimensions and values;
- FSR values;
- ToF values grouped into back, head, knee, and bottom arrays.

The schema is documented in [api.md](api.md). Changes to either the firmware serializer or TypeScript telemetry type must be mirrored on the other side.
