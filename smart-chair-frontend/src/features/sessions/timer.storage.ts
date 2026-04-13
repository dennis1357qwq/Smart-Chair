import type { Session } from "./session";

export type StoredTimerState = {
  active?: boolean;
  running?: boolean;
  session?: Session | null;
  totalSec?: number;
  remainingSec?: number;
  showMini?: boolean;
  startedAt?: number;
  pausedAccum?: number;
};

const LS_KEY = "globalTimer.v1";

export function saveTimerToStorage(data: StoredTimerState) {
  localStorage.setItem(LS_KEY, JSON.stringify(data));
}

export function loadTimerFromStorage(): StoredTimerState | null {
  try {
    const raw = localStorage.getItem(LS_KEY);
    if (!raw) return null;
    return JSON.parse(raw);
  } catch {
    return null;
  }
}

export function clearTimerStorage() {
  localStorage.removeItem(LS_KEY);
}
