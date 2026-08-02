// src/dev/mockChair.ts
import type { Telemetry } from "../shared/types/telemetry";

function clamp(n: number, a: number, b: number) {
  return Math.max(a, Math.min(b, n));
}

const labels = [
  { id: -1, label: "unknown" },
  { id: 0, label: "neutral" },

  { id: 1, label: "lean_left_neutral" },
  { id: 2, label: "lean_left_strong" },
  { id: 3, label: "lean_right_neutral" },
  { id: 4, label: "lean_right_strong" },

  { id: 5, label: "sit_front" },

  { id: 6, label: "leg_crossed_left" },
  { id: 7, label: "leg_crossed_right" },

  { id: 8, label: "neutral_no_backrest" },
  { id: 9, label: "lean_back" },

  { id: 10, label: "slouch_upper_neutral" },
  { id: 11, label: "slouch_lower_neutral" },
  { id: 12, label: "slouch_upper_left" },
  { id: 13, label: "slouch_lower_left" },
  { id: 14, label: "slouch_upper_right" },
  { id: 15, label: "slouch_lower_right" },

  { id: 16, label: "twist_left" },
  { id: 17, label: "twist_right" },

  { id: 18, label: "unoccupied" },
];

let i = 0;
let baselineRunning = false;
let baselineEndsAt = 0;

export function mockTelemetry(): Telemetry {
  i++;

  // simple posture cycle (every ~2s)
  const pick = labels[Math.floor((i / 3) % labels.length)];

  // make some fake changing sensor values
  const rows = 7;
  const cols = 7;
  const values = Array.from({ length: rows * cols }, (_, k) => {
    const wave = Math.sin((i + k) / 10) * 200;
    const base = 600 + (k % cols) * 15;
    return Math.round(clamp(base + wave, -1, 4095));
  });

  const now = Date.now();
  if (baselineRunning && now >= baselineEndsAt) baselineRunning = false;

  return {
    schema: 1,
    timestamp_ms: now,
    posId: pick.id,
    posLabel: pick.label,
    data: {
      matrix: { rows, cols, values },
      fsr: { values: [90, 210, 60] },
      tof: {
        back: [120, 135, 140, 155, 170, 160],
        head: [-1],
        knee: [-1, -1],
        bottom: [-1, -1],
      },
    },
  };
}

export function startMockBaseline(ms: number) {
  baselineRunning = true;
  baselineEndsAt = Date.now() + ms;
}

export function mockBaselineStatus() {
  const rem = baselineRunning ? Math.max(0, baselineEndsAt - Date.now()) : 0;
  return { running: baselineRunning, remainingMs: rem };
}
