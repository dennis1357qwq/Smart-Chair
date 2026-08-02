import { useNavigate } from "react-router-dom";
import { useChair } from "../core/state/ChairContext";
import { usePostureFeedback } from "../core/feedback/usePostureFeedback";

type Props = {
  variant?: "compact" | "full";
  showOpenChairButton?: boolean;
};

export default function PosturePanel({
  variant = "compact",
  showOpenChairButton = true,
}: Props) {
  const { status, telemetry, error, lastUpdatedMs } = useChair() as any;
  const fb = usePostureFeedback();
  const nav = useNavigate();

  const postureLabel = telemetry?.posLabel ?? "—";
  const postureId = telemetry?.posId ?? null;

  const chairStatusText =
    status === "online"
      ? "online"
      : status === "connecting"
        ? "connecting..."
        : status === "error"
          ? (error ?? "error")
          : (status ?? "idle");

  const titleClass =
    variant === "full"
      ? "text-lg font-semibold text-slate-900"
      : "text-sm font-medium text-slate-900";

  const valueBaseClass =
    variant === "full"
      ? "mt-2 text-2xl font-semibold"
      : "mt-1 text-sm font-medium";

  const valueClass =
    status === "online"
      ? `${valueBaseClass} ${fb.className}`
      : `${valueBaseClass} text-slate-600`;

  return (
    <div className="rounded-2xl border border-slate-200 bg-white px-4 py-3 shadow-sm">
      <div className="flex items-center justify-between">
        <div className={titleClass}>Posture</div>
        <div className="text-[11px] text-slate-500">{chairStatusText}</div>
      </div>

      <div className={valueClass}>
        {status === "online" ? (
          <>
            {postureLabel}{" "}
            {postureId !== null && (
              <span className="text-sm text-slate-500">#{postureId}</span>
            )}
          </>
        ) : (
          <span>
            {status === "connecting" ? "Connecting to chair…" : "Chair offline"}
          </span>
        )}
      </div>

      <div className="mt-2 text-[11px] text-slate-500">
        last update:{" "}
        {lastUpdatedMs
          ? `${Math.round((Date.now() - lastUpdatedMs) / 1000)}s ago`
          : "—"}
      </div>

      {showOpenChairButton ? (
        <button
          className="mt-3 text-xs text-indigo-700 hover:underline"
          onClick={() => nav("/")}
        >
          Open chair view →
        </button>
      ) : null}
    </div>
  );
}
