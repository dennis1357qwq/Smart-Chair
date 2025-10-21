import { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";
import { Dialog } from "@headlessui/react";

type Session = {
  id: string;
  title: string;
  durationMin: number;
  color?: string; // Tailwind bg-...-... (z.B. "bg-indigo-600")
  emoji?: string; // kleines Icon/Emoji
  preset?: boolean; // true für suggested
};

const LS_KEY = "customSessions.v1";

const suggested: Session[] = [
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

function loadCustom(): Session[] {
  try {
    const raw = localStorage.getItem(LS_KEY);
    if (!raw) return [];
    const arr = JSON.parse(raw);
    if (!Array.isArray(arr)) return [];
    // rudimentäre Validierung
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

function saveCustom(list: Session[]) {
  localStorage.setItem(LS_KEY, JSON.stringify(list));
}

function Tile({
  s,
  onClick,
  onDelete,
}: {
  s: Session;
  onClick: (s: Session) => void;
  onDelete?: (s: Session) => void;
}) {
  return (
    <button
      onClick={() => onClick(s)}
      className={`group relative w-44 h-28 rounded-2xl ${
        s.color ?? "bg-slate-700"
      } text-white p-4 shadow-md hover:shadow-lg transition-shadow flex flex-col items-start justify-between`}
    >
      <div className="text-2xl">{s.emoji ?? "🕒"}</div>
      <div className="text-left">
        <div className="font-semibold leading-tight">{s.title}</div>
        <div className="text-white/80 text-sm">{s.durationMin} min</div>
      </div>

      {!s.preset && onDelete && (
        <span
          onClick={(e) => {
            e.stopPropagation();
            onDelete(s);
          }}
          title="Delete"
          className="absolute top-2 right-2 opacity-0 group-hover:opacity-100 transition-opacity text-white/90 hover:text-white"
        >
          ✕
        </span>
      )}
    </button>
  );
}

function PlusTile({ onClick }: { onClick: () => void }) {
  return (
    <button
      onClick={onClick}
      className="w-44 h-28 rounded-2xl border-2 border-dashed border-slate-300 text-slate-500 hover:text-slate-700 hover:border-slate-400 transition-colors flex items-center justify-center"
    >
      <div className="text-center">
        <div className="text-2xl">＋</div>
        <div className="text-sm mt-1">Add custom</div>
      </div>
    </button>
  );
}

export default function SessionsPage() {
  const nav = useNavigate();
  const [custom, setCustom] = useState<Session[]>(() => loadCustom());

  // Modal
  const [open, setOpen] = useState(false);
  const [title, setTitle] = useState("");
  const [mins, setMins] = useState<number | "">("");

  useEffect(() => {
    saveCustom(custom);
  }, [custom]);

  const allSuggested = useMemo(() => suggested, []);

  const handleOpenCreate = () => {
    setTitle("");
    setMins("");
    setOpen(true);
  };

  const handleCreate = () => {
    const duration =
      typeof mins === "string" ? parseInt(mins || "0", 10) : mins;
    if (!title.trim() || !duration || duration < 1) return;

    const id = `custom-${Date.now()}`;
    const palette = [
      "bg-sky-600",
      "bg-teal-600",
      "bg-rose-600",
      "bg-violet-600",
      "bg-orange-600",
    ];
    const color =
      palette[(custom.length + allSuggested.length) % palette.length];

    const s: Session = {
      id,
      title: title.trim(),
      durationMin: duration,
      color,
      emoji: "📝",
      preset: false,
    };
    setCustom((prev) => [s, ...prev]);
    setOpen(false);
  };

  const handleDelete = (s: Session) => {
    setCustom((prev) => prev.filter((x) => x.id !== s.id));
  };

  const handleSelect = (s: Session) => {
    // hier kannst du state mitgeben oder Query-Params bauen
    nav(`/sessions/${s.id}`, { state: { session: s } });
  };

  return (
    <div className="p-6 space-y-8">
      <header>
        <h1 className="text-2xl font-semibold text-slate-900">Sessions</h1>
        <p className="text-slate-600">
          Pick a suggested session or create your own preset.
        </p>
      </header>

      {/* Suggested */}
      <section className="space-y-3">
        <h2 className="text-sm font-medium text-slate-600 uppercase tracking-wide">
          Suggested
        </h2>
        <div className="flex gap-4 flex-wrap">
          {allSuggested.map((s) => (
            <Tile key={s.id} s={s} onClick={handleSelect} />
          ))}
        </div>
      </section>

      {/* Your Sessions */}
      <section className="space-y-3">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-medium text-slate-600 uppercase tracking-wide">
            Your Sessions
          </h2>
          <button
            onClick={handleOpenCreate}
            className="text-sm rounded-lg px-3 py-1.5 bg-slate-900 text-white hover:bg-slate-800"
          >
            New session
          </button>
        </div>
        <div className="flex gap-4 flex-wrap">
          <PlusTile onClick={handleOpenCreate} />
          {custom.length === 0 ? (
            <div className="text-slate-500 text-sm self-center">
              No custom sessions yet.
            </div>
          ) : (
            custom.map((s) => (
              <Tile
                key={s.id}
                s={s}
                onClick={handleSelect}
                onDelete={handleDelete}
              />
            ))
          )}
        </div>
      </section>

      {/* Create Modal */}
      <Dialog
        open={open}
        onClose={() => setOpen(false)}
        className="relative z-50"
      >
        <div className="fixed inset-0 bg-black/30" aria-hidden="true" />
        <div className="fixed inset-0 flex items-center justify-center p-4">
          <Dialog.Panel className="w-full max-w-md rounded-2xl bg-white p-6 shadow-xl">
            <Dialog.Title className="text-lg font-semibold text-slate-900">
              Create custom session
            </Dialog.Title>
            <div className="mt-4 space-y-3">
              <label className="block">
                <span className="text-sm text-slate-700">Title</span>
                <input
                  value={title}
                  onChange={(e) => setTitle(e.target.value)}
                  className="mt-1 w-full rounded-lg border border-slate-300 px-3 py-2 focus:outline-none focus:ring-2 focus:ring-indigo-500"
                  placeholder="e.g., Deep Focus"
                />
              </label>
              <label className="block">
                <span className="text-sm text-slate-700">
                  Duration (minutes)
                </span>
                <input
                  type="number"
                  min={1}
                  value={mins}
                  onChange={(e) =>
                    setMins(e.target.value === "" ? "" : Number(e.target.value))
                  }
                  className="mt-1 w-full rounded-lg border border-slate-300 px-3 py-2 focus:outline-none focus:ring-2 focus:ring-indigo-500"
                  placeholder="25"
                />
              </label>
            </div>
            <div className="mt-6 flex justify-end gap-2">
              <button
                onClick={() => setOpen(false)}
                className="rounded-lg px-4 py-2 text-slate-600 hover:bg-slate-100"
              >
                Cancel
              </button>
              <button
                onClick={handleCreate}
                className="rounded-lg px-4 py-2 bg-indigo-600 text-white hover:bg-indigo-700"
              >
                Create
              </button>
            </div>
          </Dialog.Panel>
        </div>
      </Dialog>
    </div>
  );
}
