import { useState } from "react";
import { startBaseline } from "../../core/api/chairApi";
import { useChairConnection } from "../chair/useChairConnection";

export default function SettingsPage() {
  const { online, baselineText, lastError } = useChairConnection();

  const [busy, setBusy] = useState(false);
  const [toast, setToast] = useState<string | null>(null);

  async function onStartBaseline() {
    setBusy(true);
    try {
      await startBaseline();
      setToast("Baseline started.");
      window.setTimeout(() => setToast(null), 1500);
    } catch (error: unknown) {
      const message = error instanceof Error ? error.message : "unknown error";
      setToast(`Baseline failed: ${message}`);
      window.setTimeout(() => setToast(null), 2500);
    } finally {
      setBusy(false);
    }
  }

  return (
    <div className="p-6 max-w-2xl">
      <h1 className="text-2xl font-semibold text-slate-900">Settings</h1>
      <p className="text-sm text-slate-600 mt-1">
        Check chair connectivity and trigger baseline calibration.
      </p>

      <div className="mt-6 rounded-2xl border border-slate-200 bg-white p-4 shadow-sm">
        <div className="flex items-center justify-between">
          <div>
            <div className="text-sm font-medium text-slate-900">
              Chair connection
            </div>
            <div className="text-xs text-slate-600">
              Status:{" "}
              <span className={online ? "text-emerald-700" : "text-rose-700"}>
                {online ? "online" : "offline"}
              </span>
              {lastError ? (
                <span className="text-slate-500"> · {lastError}</span>
              ) : null}
            </div>
          </div>

          <div className="text-xs text-slate-600">
            Baseline:{" "}
            <span className="font-medium text-slate-900">{baselineText}</span>
          </div>
        </div>
      </div>

      <div className="mt-4 rounded-2xl border border-slate-200 bg-white p-4 shadow-sm">
        <div className="flex items-center justify-between">
          <div>
            <div className="text-sm font-medium text-slate-900">
              Baseline calibration
            </div>
            <div className="text-xs text-slate-600">
              Starts the 15s baseline sampling on the embedded unit.
            </div>
          </div>

          <button
            disabled={!online || busy}
            onClick={onStartBaseline}
            className={`rounded-xl px-3 py-2 text-sm text-white ${
              !online || busy
                ? "bg-slate-300"
                : "bg-indigo-600 hover:bg-indigo-700"
            }`}
          >
            {busy ? "Starting..." : "Start baseline"}
          </button>
        </div>
      </div>

      {toast ? (
        <div className="mt-4 rounded-xl border border-slate-200 bg-slate-50 px-3 py-2 text-sm text-slate-800">
          {toast}
        </div>
      ) : null}
    </div>
  );
}
