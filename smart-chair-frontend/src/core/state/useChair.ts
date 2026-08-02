import { useContext } from "react";
import { ChairContext, type ChairContextValue } from "./chair-context";

export function useChair(): ChairContextValue {
  const context = useContext(ChairContext);
  if (!context) {
    throw new Error("useChair must be used inside <ChairProvider>.");
  }
  return context;
}
