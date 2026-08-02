import { useEffect, useMemo, useRef, useState } from "react";
import { useChair } from "../state/useChair";
import { useTimer } from "../../features/sessions/useTimer";
import type { FeedbackRule } from "../../features/sessions/session";

export type PostureLevel = "ok" | "warn" | "bad";

function classFor(level: PostureLevel) {
  if (level === "bad") return "text-rose-600";
  if (level === "warn") return "text-amber-600";
  return "text-slate-900";
}

function matches(r: FeedbackRule, label: string, msInSameLabel: number) {
  return r.label === label && msInSameLabel >= r.afterMs;
}

export function usePostureFeedback() {
  const { telemetry } = useChair();
  const { state: timer } = useTimer();

  const label: string = telemetry?.posLabel ?? "unknown";
  const session = timer.active ? (timer.session ?? null) : null;
  const lastLabelRef = useRef(label);
  const sinceRef = useRef(Date.now());
  const [msInSameLabel, setMsInSameLabel] = useState(0);

  useEffect(() => {
    const now = Date.now();
    if (lastLabelRef.current !== label) {
      lastLabelRef.current = label;
      sinceRef.current = now;
      setMsInSameLabel(0);
    }
  }, [label]);

  useEffect(() => {
    const id = window.setInterval(() => {
      setMsInSameLabel(Date.now() - sinceRef.current);
    }, 250);
    return () => window.clearInterval(id);
  }, []);

  const level: PostureLevel = useMemo(() => {
    if (!session?.policy) return "ok";
    if (label === "unoccupied") return "ok";

    const { bad, warn } = session.policy;

    if (bad?.some((r) => matches(r, label, msInSameLabel))) return "bad";
    if (warn?.some((r) => matches(r, label, msInSameLabel))) return "warn";
    return "ok";
  }, [session, label, msInSameLabel]);

  return {
    level,
    className: classFor(level),
    msInSameLabel,
    label,
    sessionId: session?.id ?? null,
  };
}
