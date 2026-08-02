import { createContext } from "react";
import type { Session } from "./session.ts";

export type TimerState = {
  active: boolean;
  running: boolean;
  session: Session | null;
  totalSec: number;
  remainingSec: number;
  showMini: boolean;
};

export type TimerApi = {
  state: TimerState;
  start: (session: Session) => void;
  pause: () => void;
  resume: () => void;
  reset: () => void;
  hideMini: () => void;
  showMini: () => void;
};

export const TimerContext = createContext<TimerApi | null>(null);
