import React, {
  useEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import type { Telemetry } from "../../shared/types/telemetry";
import { fetchTelemetry } from "../api/chairApi";
import {
  ChairContext,
  type ChairContextValue,
  type ChairStatus,
} from "./chair-context";

type Props = {
  children: React.ReactNode;
  pollIntervalMs?: number;
};

export function ChairProvider({ children, pollIntervalMs = 800 }: Props) {
  // Initial state before the first polling attempt starts
  const [status, setStatus] = useState<ChairStatus>("idle");
  const [telemetry, setTelemetry] = useState<Telemetry | null>(null);
  const [lastUpdatedMs, setLastUpdatedMs] = useState<number | null>(null);
  const [error, setError] = useState<string | null>(null);

  // Mutable flag to trigger immediate polling cycle without re-render
  const refreshRequestedRef = useRef(false);

  const refreshNow = () => {
    refreshRequestedRef.current = true;
  };

  useEffect(() => {
    let cancelled = false;
    let timer: number | null = null;
    const ac = new AbortController();

    const loop = async () => {
      if (cancelled) return;

      setStatus((s) => (s === "online" ? "online" : "connecting"));

      try {
        const t = await fetchTelemetry(ac.signal);
        if (cancelled) return;

        setTelemetry(t);
        setLastUpdatedMs(Date.now());
        setError(null);
        setStatus("online");
      } catch (error: unknown) {
        if (cancelled) return;
        if (error instanceof DOMException && error.name === "AbortError") return;

        setStatus("error");
        setError(error instanceof Error ? error.message : String(error));
      } finally {
        if (!cancelled) {
          const immediate = refreshRequestedRef.current;
          refreshRequestedRef.current = false;

          const delay = immediate ? 50 : pollIntervalMs;
          timer = window.setTimeout(loop, delay);
        }
      }
    };

    loop();

    return () => {
      cancelled = true;
      ac.abort();
      if (timer) window.clearTimeout(timer);
    };
  }, [pollIntervalMs]);

  const value = useMemo<ChairContextValue>(() => {
    return { status, telemetry, lastUpdatedMs, error, refreshNow };
  }, [status, telemetry, lastUpdatedMs, error]);

  return (
    <ChairContext.Provider value={value}>{children}</ChairContext.Provider>
  );
}
