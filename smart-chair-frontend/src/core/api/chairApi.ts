import type { Telemetry } from "../../shared/types/telemetry";

const BASE_URL = import.meta.env.VITE_CHAIR_BASE_URL ?? "";

// if .env variable is empty, the mock version is used
function buildUrl(path: string) {
  return `${BASE_URL}${path}`;
}

async function safeFetch<T>(path: string, signal?: AbortSignal): Promise<T> {
  const res = await fetch(buildUrl(path), {
    method: "GET",
    signal,
  });

  if (!res.ok) {
    throw new Error(`HTTP ${res.status}`);
  }

  return res.json();
}

// ---- Public API ----

export async function fetchTelemetry(signal?: AbortSignal): Promise<Telemetry> {
  return safeFetch<Telemetry>("/telemetry", signal);
}

export async function startBaseline(ms = 15000, matrix = true, tof = true) {
  const url = buildUrl(
    `/baseline/start?ms=${ms}&matrix=${matrix ? 1 : 0}&tof=${tof ? 1 : 0}`,
  );

  const res = await fetch(url);
  if (!res.ok) throw new Error(`Baseline start failed`);
  return res.json();
}

export async function fetchBaselineStatus() {
  return safeFetch<{ running: boolean; remainingMs: number }>(
    "/baseline/status",
  );
}
