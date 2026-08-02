import { useEffect, useMemo, useState } from "react";
import { fetchBaselineStatus } from "../../core/api/chairApi";

export function useChairConnection() {
  const [online, setOnline] = useState(false);
  const [baseline, setBaseline] = useState<{
    running: boolean;
    remainingMs: number;
  } | null>(null);
  const [lastError, setLastError] = useState<string | null>(null);

  useEffect(() => {
    let alive = true;

    async function tick() {
      try {
        const s = await fetchBaselineStatus();
        if (!alive) return;
        setOnline(true);
        setBaseline(s);
        setLastError(null);
      } catch (error: unknown) {
        if (!alive) return;
        setOnline(false);
        setBaseline(null);
        setLastError(
          error instanceof Error ? error.message : "connection error",
        );
      }
    }

    tick();
    const id = window.setInterval(tick, 1000);
    return () => {
      alive = false;
      window.clearInterval(id);
    };
  }, []);

  const baselineText = useMemo(() => {
    if (!baseline) return "—";
    return baseline.running
      ? `running (${Math.ceil(baseline.remainingMs / 1000)}s)`
      : "idle";
  }, [baseline]);

  return { online, baselineText, lastError };
}
