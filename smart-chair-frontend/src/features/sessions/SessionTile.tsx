import type { Session } from "./session";

export function SessionTile({
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

export function PlusTile({ onClick }: { onClick: () => void }) {
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
