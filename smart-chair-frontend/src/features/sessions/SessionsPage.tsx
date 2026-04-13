import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { suggested, loadCustom, saveCustom } from "./session";
import type { Session } from "./session";
import CreateSessionModal from "./CreateSessionModal";
import { SessionTile, PlusTile } from "./SessionTile";

export default function SessionsPage() {
  const nav = useNavigate();
  const [custom, setCustom] = useState<Session[]>(() => loadCustom());
  const [open, setOpen] = useState(false);

  useEffect(() => {
    saveCustom(custom);
  }, [custom]);

  const handleOpenCreate = () => {
    setOpen(true);
  };

  const handleCreate = (session: Session) => {
    setCustom((prev) => [session, ...prev]);
  };

  const handleDelete = (s: Session) => {
    setCustom((prev) => prev.filter((x) => x.id !== s.id));
  };

  const handleSelect = (s: Session) => {
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

      <section className="space-y-3">
        <h2 className="text-sm font-medium text-slate-600 uppercase tracking-wide">
          Suggested
        </h2>
        <div className="flex gap-4 flex-wrap">
          {suggested.map((s) => (
            <SessionTile key={s.id} s={s} onClick={handleSelect} />
          ))}
        </div>
      </section>

      <section className="space-y-3">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-medium text-slate-600 uppercase tracking-wide">
            Your Sessions
          </h2>
        </div>
        <div className="flex gap-4 flex-wrap">
          <PlusTile onClick={handleOpenCreate} />
          {custom.length === 0 ? (
            <div className="text-slate-500 text-sm self-center">
              No custom sessions yet.
            </div>
          ) : (
            custom.map((s) => (
              <SessionTile
                key={s.id}
                s={s}
                onClick={handleSelect}
                onDelete={handleDelete}
              />
            ))
          )}
        </div>
      </section>
      <CreateSessionModal
        open={open}
        onClose={() => setOpen(false)}
        onCreate={handleCreate}
        index={custom.length + suggested.length}
      />
    </div>
  );
}
