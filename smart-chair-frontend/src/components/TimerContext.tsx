// src/core/timer/TimerContext.tsx
import React, {
  createContext,
  useContext,
  useEffect,
  useRef,
  useState,
} from "react";
import type { Session } from "./session";

type TimerState = {
  active: boolean;
  running: boolean;
  session: Session | null;
  totalSec: number;
  remainingSec: number;
  // UI
  showMini: boolean;
};

type TimerAPI = {
  state: TimerState;
  start: (session: Session) => void;
  pause: () => void;
  resume: () => void;
  reset: () => void;
  hideMini: () => void;
  showMini: () => void;
};

const TimerContext = createContext<TimerAPI | null>(null);

const LS_KEY = "globalTimer.v1";

// Hilfen für Persistenz
function saveToLS(
  s: Partial<TimerState> & { startedAt?: number; pausedAccum?: number }
) {
  localStorage.setItem(LS_KEY, JSON.stringify(s));
}
function loadFromLS():
  | (Partial<TimerState> & { startedAt?: number; pausedAccum?: number })
  | null {
  try {
    const raw = localStorage.getItem(LS_KEY);
    if (!raw) return null;
    return JSON.parse(raw);
  } catch {
    return null;
  }
}

export const TimerProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const tickRef = useRef<number | null>(null);

  // interne Zeitanker (für robuste Berechnung)
  const startedAtRef = useRef<number | null>(null); // Date.now() bei Start/Resume
  const pausedAccumRef = useRef<number>(0); // insgesamt pausierte ms
  const pausedSinceRef = useRef<number | null>(null);

  const [state, setState] = useState<TimerState>({
    active: false,
    running: false,
    session: null,
    totalSec: 0,
    remainingSec: 0,
    showMini: true,
  });

  // Laden (optional)
  useEffect(() => {
    const ls = loadFromLS();
    if (!ls) return;

    if (ls.active && ls.session && typeof ls.totalSec === "number") {
      startedAtRef.current = ls.startedAt ?? null;
      pausedAccumRef.current = ls.pausedAccum ?? 0;
      pausedSinceRef.current = null;
      setState((prev) => ({
        ...prev,
        active: true,
        running: !!ls.running,
        session: ls.session as Session,
        totalSec: ls.totalSec as number,
        remainingSec: ls.remainingSec ?? ls.totalSec ?? 0,
        showMini: ls.showMini ?? true,
      }));
    } else if (typeof ls.showMini === "boolean") {
      setState((p) => ({ ...p, showMini: ls.showMini! }));
    }
  }, []);

  // Ticker
  useEffect(() => {
    if (!state.active || !state.running) {
      if (tickRef.current) {
        window.clearInterval(tickRef.current);
        tickRef.current = null;
      }
      return;
    }
    if (!tickRef.current) {
      tickRef.current = window.setInterval(() => {
        // remaining = total - (now - startedAt - pausedAccum)
        const startedAt = startedAtRef.current ?? Date.now();
        const elapsedMs = Date.now() - startedAt - pausedAccumRef.current;
        const remaining = Math.max(
          0,
          state.totalSec - Math.floor(elapsedMs / 1000)
        );
        setState((s) => ({ ...s, remainingSec: remaining }));
        if (remaining === 0) {
          // Fertig
          setState((s) => ({ ...s, running: false }));
        }
      }, 1000);
    }
    return () => {
      if (tickRef.current) {
        window.clearInterval(tickRef.current);
        tickRef.current = null;
      }
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [state.active, state.running, state.totalSec]);

  function persist(partial?: Partial<TimerState>) {
    saveToLS({
      ...partial,
      // internals
      startedAt: startedAtRef.current ?? undefined,
      pausedAccum: pausedAccumRef.current,
    });
  }

  const start = (session: Session) => {
    startedAtRef.current = Date.now();
    pausedAccumRef.current = 0;
    pausedSinceRef.current = null;
    const totalSec = session.durationMin * 60;
    setState({
      active: true,
      running: true,
      session,
      totalSec,
      remainingSec: totalSec,
      showMini: true,
    });
    persist({
      active: true,
      running: true,
      session,
      totalSec,
      remainingSec: totalSec,
      showMini: true,
    });
  };

  const pause = () => {
    if (!state.active || !state.running) return;
    pausedSinceRef.current = Date.now();
    setState((s) => ({ ...s, running: false }));
    persist({ running: false, remainingSec: state.remainingSec });
  };

  const resume = () => {
    if (!state.active || state.running) return;
    if (pausedSinceRef.current) {
      pausedAccumRef.current += Date.now() - pausedSinceRef.current;
      pausedSinceRef.current = null;
    }
    setState((s) => ({ ...s, running: true }));
    persist({ running: true });
  };

  const reset = () => {
    setState((s) => ({
      ...s,
      running: false,
      remainingSec: s.totalSec,
    }));
    startedAtRef.current = Date.now();
    pausedAccumRef.current = 0;
    pausedSinceRef.current = null;
    persist({ running: false, remainingSec: state.totalSec });
  };

  const hideMini = () => {
    setState((s) => ({ ...s, showMini: false }));
    persist({ showMini: false });
  };
  const showMini = () => {
    setState((s) => ({ ...s, showMini: true }));
    persist({ showMini: true });
  };

  return (
    <TimerContext.Provider
      value={{ state, start, pause, resume, reset, hideMini, showMini }}
    >
      {children}
    </TimerContext.Provider>
  );
};

export function useTimer() {
  const ctx = useContext(TimerContext);
  if (!ctx) throw new Error("useTimer must be used within TimerProvider");
  return ctx;
}
