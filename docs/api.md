# Embedded HTTP API

The ESP32 runs an HTTP server on port 80 after connecting to Wi-Fi. Replace `<chair-ip>` with the address printed in the serial monitor.

The API currently has no authentication and sends permissive CORS headers. It is intended for use on a trusted local research network.

## Health check

```http
GET /
```

Returns `200 OK` with plain text `ok`.

## Current telemetry

```http
GET /telemetry
```

Returns the latest snapshot; it is not a stream. Distances are in millimeters. The timestamp is ESP32 uptime in milliseconds, not wall-clock time.

```json
{
  "schema": 1,
  "timestamp_ms": 123456,
  "posId": 0,
  "posLabel": "neutral",
  "data": {
    "matrix": {
      "rows": 7,
      "cols": 7,
      "values": [0]
    },
    "fsr": {
      "values": [0, 0, 0]
    },
    "tof": {
      "back": [0, 0, 0, 0, 0, 0],
      "head": [-1],
      "knee": [-1, -1],
      "bottom": [-1, -1]
    }
  }
}
```

The matrix example is abbreviated; a normal response contains 49 row-major values. Inactive ToF groups remain present in the schema and must not be interpreted as active measurements without checking the firmware configuration.

## Start baseline calibration

```http
GET /baseline/start?ms=15000&matrix=1&tof=1
POST /baseline/start?ms=15000&matrix=1&tof=1
```

Parameters:

| Name | Default | Meaning |
| --- | --- | --- |
| `ms` | `15000` | sampling duration in milliseconds |
| `matrix` | `1` | enable pressure-matrix baseline |
| `tof` | `1` | enable ToF baseline |

Returns `202 Accepted`, for example:

```json
{
  "ok": true,
  "started": true,
  "ms": 15000,
  "matrix": true,
  "tof": true
}
```

The current firmware does not validate a safe range for `ms` and does not authenticate requests. Clients should use sensible positive durations and avoid starting overlapping calibrations.

## Baseline status

```http
GET /baseline/status
```

```json
{
  "running": true,
  "remainingMs": 8200
}
```

## Example commands

```sh
curl http://<chair-ip>/
curl http://<chair-ip>/telemetry
curl 'http://<chair-ip>/baseline/start?ms=15000&matrix=1&tof=1'
curl http://<chair-ip>/baseline/status
```
