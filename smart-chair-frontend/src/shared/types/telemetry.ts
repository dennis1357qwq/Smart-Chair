export type TelemetryMatrix = {
  rows: number;
  cols: number;
  values: number[];
};

export type TelemetryFsr = {
  values: number[];
};

export type TelemetryTof = {
  back: number[];
  head: number[];
  knee: number[];
  bottom: number[];
};

export type TelemetryData = {
  matrix: TelemetryMatrix;
  fsr: TelemetryFsr;
  tof: TelemetryTof;
};

export type Telemetry = {
  schema: number;
  timestamp_ms: number;

  posId: number;
  posLabel: string;

  data: TelemetryData;
};
