import { useLocation, useNavigate } from "react-router-dom";
import { useTimer } from "../features/sessions/useTimer";
import PosturePanel from "./PosturePanel";

function formatMMSS(totalSec: number) {
  const m = Math.floor(totalSec / 60);
  const s = totalSec % 60;
  return `${String(m).padStart(2, "0")}:${String(s).padStart(2, "0")}`;
}

function routeIs(pathname: string, prefix: string) {
  return pathname === prefix || pathname.startsWith(prefix + "/");
}

export default function GlobalMiniWidget() {
  const { state: timer, pause, resume, reset, hideMini } = useTimer();
  const nav = useNavigate();
  const { pathname } = useLocation();
  const onChair = routeIs(pathname, "/");
  const timerActive = timer.active && timer.showMini && !!timer.session;
  const showTimer = timerActive;
  const showPosture = !onChair;

  if (onChair && !showTimer) return null;
  if (!showPosture && !showTimer) return null;

  return (
    <div className="fixed bottom-5 right-5 z-50">
      <div className="rounded-2xl shadow-lg border border-slate-200 bg-white/90 backdrop-blur px-4 py-3 min-w-64">
        <div className="flex items-center justify-between">
          <div className="text-sm font-semibold text-slate-900">Status</div>
          {showTimer ? (
            <button
              onClick={hideMini}
              className="rounded-md px-2 py-1 text-xs text-slate-500 hover:bg-slate-100"
              title="Hide timer"
            >
              ✕
            </button>
          ) : null}
        </div>

        {showPosture && (
          <div className="mt-2">
            <PosturePanel variant="compact" showOpenChairButton />
          </div>
        )}

        {showTimer && timer.session && (
          <div className="mt-2 rounded-xl border border-slate-200 bg-white px-3 py-2">
            <div className="flex items-center justify-between gap-3">
              <button
                onClick={() =>
                  nav(`/sessions/${timer.session!.id}`, {
                    state: { session: timer.session },
                  })
                }
                className="flex items-center gap-2"
                title="Open session"
              >
                <span className="text-xl">{timer.session.emoji ?? "🕒"}</span>
                <div className="text-left">
                  <div className="text-sm font-medium text-slate-900">
                    {timer.session.title}
                  </div>
                  <div className="text-xs text-slate-600">
                    {formatMMSS(timer.remainingSec)}
                  </div>
                </div>
              </button>

              <div className="flex items-center gap-2">
                {timer.running ? (
                  <button
                    onClick={pause}
                    className="rounded-md px-2 py-1 text-xs bg-amber-600 text-white hover:bg-amber-700"
                  >
                    Pause
                  </button>
                ) : (
                  <button
                    onClick={resume}
                    className="rounded-md px-2 py-1 text-xs bg-indigo-600 text-white hover:bg-indigo-700"
                  >
                    Resume
                  </button>
                )}
                <button
                  onClick={reset}
                  className="rounded-md px-2 py-1 text-xs bg-slate-100 text-slate-700 hover:bg-slate-200"
                  title="Reset"
                >
                  Reset
                </button>
              </div>
            </div>

            <div className="mt-2 h-2 w-full bg-slate-200 rounded-full overflow-hidden">
              <div
                className={`${
                  timer.session.color ?? "bg-slate-800"
                } h-full transition-all`}
                style={{
                  width: `${Math.round(
                    (1 - timer.remainingSec / timer.totalSec) * 100,
                  )}%`,
                }}
              />
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
