import React, {
  useEffect,
  useRef,
  useState,
} from "react";
import type { Session } from "./session.ts";
import { getSessionById } from "./session.ts";
import { saveTimerToStorage, loadTimerFromStorage } from "./timer.storage.ts";
import { TimerContext, type TimerState } from "./timer-context.ts";

export const TimerProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const tickRef = useRef<number | null>(null);
  const startedAtRef = useRef<number | null>(null);
  const pausedAccumRef = useRef<number>(0);
  const pausedSinceRef = useRef<number | null>(null);

  const [state, setState] = useState<TimerState>({
    active: false,
    running: false,
    session: null,
    totalSec: 0,
    remainingSec: 0,
    showMini: true,
  });

  useEffect(() => {
    const ls = loadTimerFromStorage();
    if (!ls) return;

    if (ls.active && ls.session && typeof ls.totalSec === "number") {
      const s = ls.session as Session;
      const canonical = getSessionById(s.id) ?? s;

      startedAtRef.current = ls.startedAt ?? null;
      pausedAccumRef.current = ls.pausedAccum ?? 0;
      pausedSinceRef.current = null;

      setState((prev) => ({
        ...prev,
        active: true,
        running: !!ls.running,
        session: canonical,
        totalSec: ls.totalSec as number,
        remainingSec: ls.remainingSec ?? ls.totalSec ?? 0,
        showMini: ls.showMini ?? true,
      }));
    } else if (typeof ls.showMini === "boolean") {
      setState((p) => ({ ...p, showMini: ls.showMini! }));
    }
  }, []);

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
          state.totalSec - Math.floor(elapsedMs / 1000),
        );
        setState((s) => ({ ...s, remainingSec: remaining }));
        if (remaining === 0) {
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
  }, [state.active, state.running, state.totalSec]);

  function persist(partial?: Partial<TimerState>) {
    saveTimerToStorage({
      ...partial,
      startedAt: startedAtRef.current ?? undefined,
      pausedAccum: pausedAccumRef.current,
    });
  }

  const start = (session: Session) => {
    const canonical = getSessionById(session.id) ?? session;

    startedAtRef.current = Date.now();
    pausedAccumRef.current = 0;
    pausedSinceRef.current = null;

    const totalSec = canonical.durationMin * 60;

    setState({
      active: true,
      running: true,
      session: canonical,
      totalSec,
      remainingSec: totalSec,
      showMini: true,
    });

    persist({
      active: true,
      running: true,
      session: canonical,
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
