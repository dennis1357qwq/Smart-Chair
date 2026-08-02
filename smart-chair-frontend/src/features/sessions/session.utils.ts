import type { Session, FeedbackRule } from "./session";
import { SESSION_COLOR_PALETTE } from "./session.constants";

export function toggleStringInList(value: string, list: string[]) {
  return list.includes(value)
    ? list.filter((x) => x !== value)
    : [...list, value];
}

export function createCustomSession({
  title,
  durationMin,
  badLabels,
  index,
}: {
  title: string;
  durationMin: number;
  badLabels: string[];
  index: number;
}): Session {
  const color = SESSION_COLOR_PALETTE[index % SESSION_COLOR_PALETTE.length];

  const badRules: FeedbackRule[] = badLabels.map((label) => ({
    label,
    afterMs: 0,
  }));

  return {
    id: `custom-${Date.now()}`,
    title: title.trim(),
    durationMin,
    color,
    emoji: "📝",
    preset: false,
    policy: {
      bad: badRules,
      warn: [],
    },
  };
}
