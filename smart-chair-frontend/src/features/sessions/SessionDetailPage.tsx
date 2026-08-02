import { useMemo } from "react";
import { useLocation, useNavigate, useParams } from "react-router-dom";
import { getSessionById, type Session } from "./session.ts";
import { useTimer } from "./TimerContext.tsx";

function formatMMSS(totalSec: number) {
  const m = Math.floor(totalSec / 60);
  const s = totalSec % 60;
  return `${String(m).padStart(2, "0")}:${String(s).padStart(2, "0")}`;
}

export default function SessionDetailPage() {
  const { id } = useParams();
  const nav = useNavigate();
  const loc = useLocation() as { state?: { session?: Session } };

  const session = useMemo<Session | undefined>(() => {
    if (loc.state?.session) return loc.state.session;
    if (id) return getSessionById(id);
    return undefined;
  }, [id, loc.state]);

  if (!session) {
    return (
      <div className="p-6">
        <div className="max-w-xl">
          <h1 className="text-xl font-semibold text-slate-900">
            Session not found
          </h1>
          <p className="text-slate-600 mt-2">
            The requested session doesn’t exist.
          </p>
          <button
            onClick={() => nav("/sessions")}
            className="mt-4 rounded-lg px-4 py-2 bg-slate-900 text-white hover:bg-slate-800"
          >
            Back to Sessions
          </button>
        </div>
      </div>
    );
  }

  const { state: timer, start, pause, resume, reset } = useTimer();
  const isThisSessionActive = timer.active && timer.session?.id === session.id;
  const isRunning = isThisSessionActive && timer.running;

  const fallbackTotalSec = session.durationMin * 60;
  const totalSec = isThisSessionActive ? timer.totalSec : fallbackTotalSec;
  const remaining = isThisSessionActive ? timer.remainingSec : fallbackTotalSec;
  const progress = totalSec > 0 ? 1 - remaining / totalSec : 0;

  const handleStartPause = () => {
    if (!isThisSessionActive) {
      start(session);
      return;
    }
    isRunning ? pause() : resume();
  };

  const handleReset = () => {
    if (isThisSessionActive) {
      reset();
    } else {
    }
  };

  return (
    <div className="relative min-h-[calc(100vh-4rem)] p-6 overflow-hidden">
      <div
        className={`pointer-events-none absolute inset-0 ${
          session.color ?? "bg-slate-700"
        } opacity-20`}
      />
      <div className="pointer-events-none absolute inset-0 flex items-center justify-center select-none">
        <div className="text-[35vmin] blur-[6px] opacity-20">
          {session.emoji ?? "🕒"}
        </div>
      </div>
      <div className="relative">
        <button
          onClick={() => nav(-1)}
          className="mb-6 rounded-lg px-3 py-1.5 text-sm text-slate-600 hover:bg-slate-100"
        >
          ← Back
        </button>

        <div className="max-w-3xl space-y-6">
          <header className="space-y-2">
            <h1 className="text-3xl font-semibold text-slate-900 flex items-center gap-3">
              <span className="text-3xl">{session.emoji ?? "🕒"}</span>
              {session.title}
            </h1>
            <p className="text-slate-600">
              {session.preset
                ? "A suggested preset session."
                : "Your custom session."}
            </p>
          </header>
          <div className="rounded-2xl bg-white/80 backdrop-blur shadow p-6">
            <div className="flex items-center justify-between">
              <div className="text-6xl font-semibold tabular-nums tracking-tight">
                {formatMMSS(remaining)}
              </div>
              <div className="text-slate-500">
                Duration:{" "}
                <span className="font-medium text-slate-700">
                  {Math.round(totalSec / 60)} min
                </span>
              </div>
            </div>
            <div className="mt-6">
              <div className="h-3 w-full bg-slate-200 rounded-full overflow-hidden">
                <div
                  className={`h-full ${
                    session.color ?? "bg-slate-800"
                  } transition-all`}
                  style={{ width: `${Math.round(progress * 100)}%` }}
                />
              </div>
              <div className="mt-2 text-sm text-slate-600">
                {Math.round(progress * 100)}% complete
              </div>
            </div>
            <div className="mt-6 flex gap-3">
              <button
                onClick={handleStartPause}
                className={`rounded-lg px-5 py-2 text-white ${
                  isRunning
                    ? "bg-amber-600 hover:bg-amber-700"
                    : "bg-indigo-600 hover:bg-indigo-700"
                }`}
              >
                {isThisSessionActive
                  ? isRunning
                    ? "Pause"
                    : "Resume"
                  : "Start"}
              </button>
              <button
                onClick={handleReset}
                className="rounded-lg px-5 py-2 text-slate-700 bg-slate-100 hover:bg-slate-200"
              >
                Reset
              </button>
            </div>
          </div>
          <section className="prose max-w-2xl">
            <h2>About this session</h2>
            <p>
              Use this session to build an ergonomic habit. Keep your shoulders
              relaxed, feet flat on the floor and avoid leaning forward for too
              long. Consider short micro breaks.
            </p>
          </section>
        </div>
      </div>
    </div>
  );
}
