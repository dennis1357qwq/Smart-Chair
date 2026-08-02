import { createContext } from "react";
import type { Telemetry } from "../../shared/types/telemetry";

export type ChairStatus = "idle" | "connecting" | "online" | "error";

export type ChairContextValue = {
  status: ChairStatus;
  telemetry: Telemetry | null;
  lastUpdatedMs: number | null;
  error: string | null;
  refreshNow: () => void;
};

export const ChairContext = createContext<ChairContextValue | null>(null);
