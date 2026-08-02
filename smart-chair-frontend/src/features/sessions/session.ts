export type FeedbackRule = {
  label: string;
  afterMs: number;
};

export type SessionPolicy = {
  warn: FeedbackRule[];
  bad: FeedbackRule[];
};

export type Session = {
  id: string;
  title: string;
  durationMin: number;
  color?: string;
  emoji?: string;
  preset?: boolean;
  policy?: SessionPolicy;
};

export const LS_KEY = "customSessions.v1";

export const suggested: Session[] = [
  {
    id: "posture-quick",
    title: "Posture Quick Check",
    durationMin: 2,
    color: "bg-indigo-600",
    emoji: "🪑",
    preset: true,
    policy: {
      bad: [
        { label: "slouch_upper_left", afterMs: 0 },
        { label: "slouch_upper_right", afterMs: 0 },
        { label: "slouch_upper_neutral", afterMs: 0 },
        { label: "slouch_lower_left", afterMs: 0 },
        { label: "slouch_lower_right", afterMs: 0 },
        { label: "slouch_lower_neutral", afterMs: 0 },
      ],
      warn: [
        { label: "lean_left_neutral", afterMs: 12000 },
        { label: "lean_left_strong", afterMs: 12000 },
        { label: "lean_right_neutral", afterMs: 12000 },
        { label: "lean_right_strong", afterMs: 12000 },
      ],
    },
  },
  {
    id: "focus-25",
    title: "Focus 25",
    durationMin: 25,
    color: "bg-emerald-600",
    emoji: "🎯",
    preset: true,
    policy: {
      bad: [
        { label: "slouch_upper_left", afterMs: 0 },
        { label: "slouch_upper_right", afterMs: 0 },
        { label: "slouch_upper_neutral", afterMs: 0 },
        { label: "slouch_lower_left", afterMs: 0 },
        { label: "slouch_lower_right", afterMs: 0 },
        { label: "slouch_lower_neutral", afterMs: 0 },
      ],
      warn: [
        { label: "lean_left_neutral", afterMs: 12000 },
        { label: "lean_left_strong", afterMs: 12000 },
        { label: "lean_right_neutral", afterMs: 12000 },
        { label: "lean_right_strong", afterMs: 12000 },
      ],
    },
  },
  {
    id: "microbreak",
    title: "Micro Break",
    durationMin: 1,
    color: "bg-amber-600",
    emoji: "☕",
    preset: true,
    policy: {
      bad: [],
      warn: [],
    },
  },
  {
    id: "stretch-5",
    title: "Stretch 5",
    durationMin: 5,
    color: "bg-fuchsia-600",
    emoji: "🧘",
    preset: true,
    policy: {
      bad: [
        { label: "slouch_upper_left", afterMs: 0 },
        { label: "slouch_upper_right", afterMs: 0 },
        { label: "slouch_upper_neutral", afterMs: 0 },
        { label: "slouch_lower_left", afterMs: 0 },
        { label: "slouch_lower_right", afterMs: 0 },
        { label: "slouch_lower_neutral", afterMs: 0 },
      ],
      warn: [
        { label: "lean_left_neutral", afterMs: 12000 },
        { label: "lean_left_strong", afterMs: 12000 },
        { label: "lean_right_neutral", afterMs: 12000 },
        { label: "lean_right_strong", afterMs: 12000 },
        { label: "sit_front", afterMs: 0 },
        { label: "neutral_no_backrest", afterMs: 0 },
      ],
    },
  },
];

export function loadCustom(): Session[] {
  try {
    const raw = localStorage.getItem(LS_KEY);
    if (!raw) return [];
    const arr = JSON.parse(raw);
    if (!Array.isArray(arr)) return [];
    return arr.filter(
      (x) =>
        x &&
        typeof x.id === "string" &&
        typeof x.title === "string" &&
        typeof x.durationMin === "number"
    );
  } catch {
    return [];
  }
}

export function saveCustom(list: Session[]) {
  localStorage.setItem(LS_KEY, JSON.stringify(list));
}

export function getSessionById(id: string): Session | undefined {
  const s = suggested.find((x) => x.id === id);
  if (s) return s;
  const custom = loadCustom();
  return custom.find((x) => x.id === id);
}
