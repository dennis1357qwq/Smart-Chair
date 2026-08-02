# Known limitations and research status

This project is a functional research prototype. The following constraints are important when interpreting results or planning extensions.

## Validation

- No controlled multi-participant user study has been conducted.
- There are no classification accuracy, precision, recall, or ground-truth agreement metrics.
- Behavioral effects of session feedback have not been evaluated.
- Current observations are based on exploratory technical testing.

The chair must not be presented as a validated medical or diagnostic device.

## Calibration dependency

Classification is relative to a roughly 15-second user baseline. Results may degrade when:

- the user moves or does not sit neutrally during calibration;
- another person uses the chair without recalibrating;
- chair height, seat depth, backrest, or lumbar support changes afterward;
- sensors or upholstery move;
- long-term sensor drift occurs.

Baseline drift is not detected automatically.

## Classification boundaries

Thresholds and zone rules were tuned empirically. Exploratory results reported in the thesis indicate:

- occupancy detection and forward sitting are comparatively robust;
- stronger lateral differentiation and lean-back detection remain unstable;
- upper slouch detection can produce false positives;
- lower slouch detection is inconsistent;
- leg-crossing and torso-twist identifiers are not finalized in the active mapping.

The rule-based design is transparent and suitable for prototyping, but complexity increases quickly as postures and modalities are added.

## Inactive modalities

The telemetry model supports back, head, knee, bottom, and FSR data, but the current classifier uses only the pressure matrix and backrest ToF features. The second ToF multiplexer and non-backrest ToF registrations are disabled in `src/main.cpp`.

## Firmware and network behavior

- Startup blocks indefinitely until Wi-Fi connects.
- The HTTP API has no authentication.
- CORS accepts all origins.
- Calibration parameters are not range-validated.
- The API is synchronous and polling-based.
- Serial debug output is verbose and always enabled in the main loop.

Use the prototype on a trusted local network.

## Software quality status

- Native host tests cover the platform-independent posture interpretation pipeline without requiring an ESP32.
- Physical sensor drivers, embedded networking, and the HTTP server are not covered by the native suite.
- No automated frontend component tests or hardware-in-the-loop tests are present.
- The frontend production build and ESLint check succeed.
- The Electron wrapper is incomplete/experimental.
- Python visualization dependencies are defined in `requirements.txt`; `matrix-viewer.py` and `multi-viewer.py` still use locally configured serial ports.

## Recommended research sequence

Before adding machine-learning classification or more sensors:

1. freeze and document the physical sensor configuration;
2. stabilize calibration and the current core posture states;
3. collect repeated, annotated data from multiple users;
4. establish quantitative baseline metrics;
5. only then compare rule-based and data-driven classifiers.
