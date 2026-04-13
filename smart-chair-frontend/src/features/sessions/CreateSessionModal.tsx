import { useState } from "react";
import { Dialog } from "@headlessui/react";
import { ALL_POSTURE_LABELS } from "./session.constants";
import { createCustomSession } from "./session.utils";
import type { Session } from "./session";

function toggleInList(
  value: string,
  list: string[],
  setList: (v: string[]) => void,
) {
  setList(
    list.includes(value) ? list.filter((x) => x !== value) : [...list, value],
  );
}

export default function CreateSessionModal({
  open,
  onClose,
  onCreate,
  index,
}: {
  open: boolean;
  onClose: () => void;
  onCreate: (s: Session) => void;
  index: number;
}) {
  const [title, setTitle] = useState("");
  const [durationInput, setDurationInput] = useState("");
  const [badLabels, setBadLabels] = useState<string[]>([]);

  const resetForm = () => {
    setTitle("");
    setDurationInput("");
    setBadLabels([]);
  };

  const handleCreate = () => {
    const duration = parseInt(durationInput, 10);
    if (!title.trim() || !duration || duration < 1) return;

    const session = createCustomSession({
      title,
      durationMin: duration,
      badLabels,
      index,
    });

    onCreate(session);
    resetForm();
    onClose();
  };

  return (
    <Dialog open={open} onClose={onClose} className="relative z-50">
      <div className="fixed inset-0 bg-black/30" aria-hidden="true" />
      <div className="fixed inset-0 flex items-center justify-center p-4">
        <Dialog.Panel className="w-full max-w-md rounded-2xl bg-white p-6 shadow-xl">
          <Dialog.Title className="text-lg font-semibold text-slate-900">
            Create Session
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
              <span className="text-sm text-slate-700">Duration (minutes)</span>
              <input
                type="number"
                min={1}
                value={durationInput}
                onChange={(e) => setDurationInput(e.target.value)}
                className="mt-1 w-full rounded-lg border border-slate-300 px-3 py-2 focus:outline-none focus:ring-2 focus:ring-indigo-500"
                placeholder="25"
              />
            </label>
          </div>

          <div className="mt-4">
            <div className="flex items-center justify-between">
              <span className="text-sm text-slate-700">Trying to avoid</span>
              <span className="text-[11px] text-slate-500">
                {badLabels.length} selected
              </span>
            </div>

            <div className="mt-2 flex flex-wrap gap-2">
              {ALL_POSTURE_LABELS.map((lbl) => {
                const active = badLabels.includes(lbl);
                return (
                  <button
                    key={lbl}
                    type="button"
                    onClick={() => toggleInList(lbl, badLabels, setBadLabels)}
                    className={[
                      "rounded-full px-3 py-1 text-xs border transition",
                      active
                        ? "bg-rose-50 border-rose-300 text-rose-700"
                        : "bg-white border-slate-200 text-slate-700 hover:bg-slate-50",
                    ].join(" ")}
                    title="Add to 'bad' policy"
                  >
                    {lbl}
                  </button>
                );
              })}
            </div>

            <p className="mt-2 text-[11px] text-slate-500">
              Selected labels will be stored in the session policy as{" "}
              <span className="font-medium">bad</span> (red).
            </p>
          </div>
          <div className="mt-6 flex justify-end gap-2">
            <button
              onClick={() => {
                resetForm();
                onClose();
              }}
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
  );
}
