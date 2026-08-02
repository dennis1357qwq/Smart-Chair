import { useContext } from "react";
import { TimerContext, type TimerApi } from "./timer-context.ts";

export function useTimer(): TimerApi {
  const context = useContext(TimerContext);
  if (!context) {
    throw new Error("useTimer must be used within TimerProvider");
  }
  return context;
}
