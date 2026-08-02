import PosturePanel from "../../ui/PosturePanel";
import { useTimer } from "../sessions/TimerContext";

export default function ChairPage() {
  const { state: timer } = useTimer();
  const timerActive = timer.active && !!timer.session;

  return (
    <div className="p-6 max-w-3xl">
      <h1 className="text-2xl font-semibold text-slate-900">Chair</h1>
      <p className="text-sm text-slate-600 mt-1">
        Live posture classification from the embedded unit.
      </p>

      <div className="mt-6">
        <PosturePanel variant="full" showOpenChairButton={false} />
      </div>

      {timerActive ? (
        <div className="mt-6 text-sm text-slate-600">
          A session is active. The timer is shown in the global widget when you
          leave this page.
        </div>
      ) : null}
    </div>
  );
}
