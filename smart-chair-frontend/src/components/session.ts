// src/core/sessions.ts
export type Session = {
  id: string;
  title: string;
  durationMin: number;
  color?: string; // Tailwind bg-... (z.B. "bg-indigo-600")
  emoji?: string; // kleines Icon/Emoji
  preset?: boolean; // true = suggested
  // optional: thumbnailUrl?: string; // falls du echte Bilder willst
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
  },
  {
    id: "focus-25",
    title: "Focus 25",
    durationMin: 25,
    color: "bg-emerald-600",
    emoji: "🎯",
    preset: true,
  },
  {
    id: "microbreak",
    title: "Micro Break",
    durationMin: 1,
    color: "bg-amber-600",
    emoji: "☕",
    preset: true,
  },
  {
    id: "stretch-5",
    title: "Stretch 5",
    durationMin: 5,
    color: "bg-fuchsia-600",
    emoji: "🧘",
    preset: true,
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
